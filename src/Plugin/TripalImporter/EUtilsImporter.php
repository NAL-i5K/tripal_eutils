<?php

namespace Drupal\tripal_eutils\Plugin\TripalImporter;

use Drupal\tripal_chado\TripalImporter\ChadoImporterBase;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;


/**
 * Tripal EUtils Importer implementation of the TripalImporterBase.
 *
 *  @TripalImporter(
 *    id = "eutils_loader",
 *    label = @Translation("NCBI EUtils Accession Loader"),
 *    description = @Translation("Import NCBI BioProjects, BioSamples, or Assemblies into Chado"),
 *    upload_description = @Translation("Not applicable"),
 *    upload_title = @Translation("Not applicable"),
 *    use_analysis = False,
 *    require_analysis = False,
 *    button_text = @Translation("Import NCBI Record"),
 *    file_upload = False,
 *    file_remote = False,
 *    file_local = False,
 *    file_required = False,
 *  )
 */
class EUtilsImporter extends ChadoImporterBase implements ContainerFactoryPluginInterface {

  /**
   * Used to store the manager so we can create various buddies
   */
  protected object $buddy_manager;

  /**
   * Provide the dbxref buddy instance
   */
  protected object $dbxref_buddy;

  /**
   * Provide the cvterm buddy instance
   */
  protected object $cvterm_buddy;

  /**
   * Provide the property buddy instance
   */
  protected object $property_buddy;

  /**
   * Implements ContainerFactoryPluginInterface->create().
   *
   * We are injecting an additional dependency here, the
   * ChadoBuddyPluginManager.
   *
   * Since we have implemented the ContainerFactoryPluginInterface this static function
   * will be called behind the scenes when a Plugin Manager uses createInstance(). Specifically
   * this method is used to determine the parameters to pass to the contructor.
   *
   * @param \Symfony\Component\DependencyInjection\ContainerInterface $container
   * @param array $configuration
   * @param string $plugin_id
   * @param mixed $plugin_definition
   *
   * @return static
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('tripal_chado.database')
#,
#      $container->get('tripal_chado.chado_buddy')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition,
                              \Drupal\tripal_chado\Database\ChadoConnection $connection
#, ChadoBuddyPluginManager $buddy_manager
) {
    parent::__construct($configuration, $plugin_id, $plugin_definition, $connection);
#    $this->buddy_manager = $buddy_manager;
#    $this->dbxref_buddy = $this->buddy_manager->createInstance('chado_dbxref_buddy', []);
#    $this->cvterm_buddy = $this->buddy_manager->createInstance('chado_cvterm_buddy', []);
#    $this->property_buddy = $this->buddy_manager->createInstance('chado_property_buddy', []);
  }

  /**
   * {@inheritDoc}
   */
  public function form($form, &$form_state) {
    // Call the parent form to ensure Chado is handled properly.
    $form = parent::form($form, $form_state);

    $form_state_values = $form_state->getValues();

    $form['instructions'] = [
      '#markup' => t('<p>Please enter one or more accessions and specify a database.</p>
                  <p>Press the <b>Preview First Record</b> button to view the
                  retrieved data and metadata.  Pressing <b>Import NCBI
                  Record</b> will create the record.</p>'),
    ];

    $db_choices = [
      'bioproject' => 'BioProject',
      'biosample' => 'BioSample',
      'assembly' => 'Assembly',
    ];

    $form['db'] = [
      '#type' => 'radios',
      '#title' => t('NCBI Database'),
      '#description' => t('The database to query.'),
      '#options' => $db_choices,
    ];

    $form['accession'] = [
      '#type' => 'textarea',
      '#rows' => 1,
      '#title' => t('NCBI Accession Number(s)'),
      '#description' => t('Valid examples: (BioSample 744358 120060 SAMN02261463),'
                        . ' (Assembly 91111, 751381, GCA_000516895.1),'
                        . ' (BioProject 12384, 394253, 66853, PRJNA185471).'
                        . ' Separate multiple accessions with comma, semicolon, or space.'),
    ];

    $form['callback'] = [
      '#type' => 'button',
      '#value' => 'Preview First Record',
    ];

    if (isset($form_state_values['parsed'])) {
      $form['data'] = [
        '#type' => 'fieldset',
        '#title' => 'Data',
      ];

      $form['data'][] = $form_state_values['parsed'];
    }

    $form['options'] = [
      '#type' => 'fieldset',
      '#title' => 'Options',
    ];

    $form['options']['linked_records'] = [
      '#type' => 'checkbox',
      '#title' => t('Create Linked Records'),
      '#description' => t('Each accession links to other NCBI databases:'
                        . ' you can create those chado records as well.'),
      '#default_value' => 1,
    ];

    return $form;
  }

  /**
   * {@inheritDoc}
   * @todo only run the EUtils check if submitted.
   */
  public function formValidate($form, &$form_state) {

    $form_state_values = $form_state->getValues();

    $db = $form_state_values['db'];
    $accession = trim($form_state_values['accession']);

    if (!$db) {
      $form_state->setErrorByName('db', t('please select a valid db'));
    }

    if (!$accession) {
      $form_state->setErrorByName('accession', t('please enter an accession'));
    }
    if ($db and $accession) {
      // If multiple accessions, only preview the first one.
      $accession = preg_replace('/[;, ].*/', '', $accession);
      $eutils_connection = new \EUtils();
      try {
        $eutils_connection->setPreview();
        $parsed = $eutils_connection->get($db, $accession);
        $form_state->setValue('parsed', $parsed);
      }
      catch (\Exception $e) {
        \Drupal::service('tripal.logger')->error($e->getMessage());
      }
    }
  }

  /**
   * {@inheritdoc}
   */
  public function formSubmit($form, &$form_state) {

  }

  /**
   * {@inheritDoc}
   */
  public function run() {
    $arguments = $this->arguments['run_args'];
    $db = $arguments['db'];
    $accessions = $arguments['accession'];
    $create_linked_records = $arguments['linked_records'];

    $job = $this->job;

    $this->tripal_eutils_create_records($db, $accessions, $create_linked_records, $job);
  }

  /**
   * {@inheritdoc}
   */
  public function postRun() {

  }

  /**
   * Runs the EUtils class to create the records for an NCBI entry.
   *
   * @param string $db
   *   The database name (eg, biosample, assembly or bioproject).
   * @param string $accession
   *   The numeric accession, or accessions separated by delimiter of comma, semicolon, or space.
   * @param bool $create_linked_records
   *   Whether to create linked records or not.
   * @param $job
   *   If present, the Tripal job running this importer
   */
  function tripal_eutils_create_records(string $db, string $accession, bool $create_linked_records, $job = NULL) {
    $accs = preg_split('/[,; ]+/', trim($accession));
    foreach ($accs as $acc) {
      $attempts = 3;
      $success = FALSE;
      while ((!$success) and ($attempts)) {
        try {
          $eutils = new EUtils($create_linked_records, $job);
          $eutils->get($db, $acc);
          $success = TRUE;
        }
        catch (Exception $exception) {

          $message = $exception->getMessage();
          // Distinguish between download error and SQL error, e.g. from reloading same assembly twice.
          // Download error: "ERROR Could not make request: Status: 400"
          // SQL error: "SQLSTATE[25P02]: In failed sql transaction: 7 ERROR:  current transaction is aborted, commands ignored until end of transaction block"
          if (preg_match('/SQL/', $message)) {
            $attempts = 1;
          }
          if ($attempts > 1) {
            tripal_report_error('tripal_eutils', TRIPAL_WARNING, 'Download error, retrying '.$message, [], ['print' => TRUE, 'job' => $job]);
            sleep(1);
          }
          else {
            tripal_report_error('tripal_eutils', TRIPAL_ERROR, $message, [], ['print' => TRUE, 'job' => $job]);
          }
        }
        $attempts--;
      }
    }
  }
}

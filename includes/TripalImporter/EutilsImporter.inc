<?php

/**
 * EUtils Importer for loading remote NCBI accessions.
 */
class EutilsImporter extends TripalImporter {


  public static $name = 'NCBI EUtils Accession loader';
  public static $description = 'Import NCBI BioProjects, BioSamples, or Assemblies into Chado';
  public static $machine_name = 'eutils_loader';
  public static $use_analysis = FALSE;
  public static $button_text = 'Import NCBI Record';
  public static $methods = [
    'file_upload' => FALSE,
    'file_local' => FALSE,
    'file_remote' => FALSE,
  ];
  public static $file_required = FALSE;

  /**
   * Implements hook_form().
   *
   * @see TripalImporter::form()
   */
  public function form($form, &$form_state) {

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
      '#description' => t('Valid examples: (BioSample 744358 120060 SAMN02261463), (Assembly 91111, 751381, GCA_000516895.1), (BioProject 12384, 394253, 66853, PRJNA185471).
                       Separate multiple accessions with comma, semicolon, or space.'),
    ];

    $form['callback'] = [
      '#type' => 'button',
      '#value' => "Preview First Record",
    ];

    if (isset($form_state['values']['parsed'])) {
      $form['data'] = [
        '#type' => 'fieldset',
        '#title' => 'Data',
      ];

      $form['data'][] = $form_state['values']['parsed'];

    }

    $form['options'] = [
      '#type' => 'fieldset',
      '#title' => "Options",
    ];

    $form['options']['linked_records'] = [
      '#type' => 'checkbox',
      '#title' => t('Create Linked Records'),
      '#description' => t('Each accession links to other NCBI databases: 
                          you can create those chado records as well.'),
      '#default_value' => 1,
    ];

    return $form;
  }

  /**
   * Validate form keys and check the accession on NCBI.
   *
   * @see TripalImporter::formValidate()
   * TODO only run the EUTils check if submitted.
   */
  public function formValidate($form, &$form_state) {

    $vals = $form_state['values'];

    $db = $vals['db'];
    $accession = trim($vals['accession']);

    if (!$db) {
      form_set_error('db', 'please select a valid db');
    }

    if (!$accession) {
      form_set_error('accession', 'please enter an accession');
    }

    if (!$db or !$accession) {
      return;
    }

    // If multiple accessions, only preview the first one.
    $accession = preg_replace('/[;, ].*/', '', $accession);
    $connection = new \EUtils();
    try {
      $connection->setPreview();
      $parsed = $connection->get($db, $accession);
      $form_state['values']['parsed'] = $parsed;
    }
    catch (\Exception $e) {
      tripal_set_message($e->getMessage(), TRIPAL_ERROR);
      return;
    }
  }

  /**
   * Runs the importer.
   *
   * @see TripalImporter::run()
   */
  public function run() {
    $arguments = $this->arguments['run_args'];
    $db = $arguments['db'];
    $accessions = $arguments['accession'];
    $create_linked_records = $arguments['linked_records'];

    $job = $this->job;

    tripal_eutils_create_records($db, $accessions, $create_linked_records, $job);
  }

}

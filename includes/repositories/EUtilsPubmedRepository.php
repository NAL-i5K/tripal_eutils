<?php

/**
 * Takes parsed pubmed XMLs and creates chado.pub.  Uses core API.
 *
 * @ingroup repositories.
 */
class EUtilsPubmedRepository extends EUtilsRepository {

  /**
   * Required attributes when using the create method.
   *
   * @var array
   */
  protected $required_fields = [
    'name',
    'description',
  ];

  protected $base_table = 'pub';

  /**
   * Creates a publication using the core API.
   *
   * @param array $data
   *   Data from bioproject parser.
   *
   * @return pub
   *   A Chado publication record object.
   **/
  public function create(array $data) {
    $pmid = $data['Publication Dbxref'] ?? NULL;
    if (!$pmid) {
      return;
    }
    // We will call the publication importer directly
    $arguments = [
      'run_args' => [
        'criteria' => [
          'criteria' => [
            1 => [
              'search_terms' => $pmid,
              'scope' => 'id',
              'is_phrase' => 0,
              'operation' => '',
            ],
          ],
          'days' => '',
          'disabled' => 0,
          'do_contact' => 0,
          'form_state_user_input' => [
            'plugin_id' => 'tripal_pub_library_PMID'
          ],
          'loader_name' => 'internal',
          'num_criteria' => 1,
          'remote_db' => 'PMID',
          'pub_import_id' => NULL,
        ],
        'schema_name' => $this->chado->getSchemaName(),
      ],
    ];
    /** @var Drupal\tripal\TripalImporter\PluginManagers\TripalImporterManager **/
    $importer_manager = \Drupal::service('tripal.importer');
    $pub_instance = $importer_manager->createInstance('pub_search_query_loader', []);
// Temporary message
if (!method_exists($pub_instance, 'setArgumentsz')) {
  $this->logger->warning('Skipped importing publication, this is dependent on Tripal pull request 2151');
  return NULL;
}
    $pub_instance->setArguments($arguments);
    $result = $pub_instance->run();

    $uname = $data['Citation'];
    $pub = $this->chado->select('1:pub', 'p')
      ->fields('p')
      ->condition('p.uniquename', $uname)
      ->execute()
      ->fetchObject();
    return $pub;
  }

}

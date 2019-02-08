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
   **/
  public function create(array $data) {
    module_load_include('inc', 'tripal_chado', '/includes/loaders/tripal_chado.pub_importers');

    tripal_pub_add_publications([$data], FALSE);
  }

}

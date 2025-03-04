<?php

/**
 * Class EUtilsBioSampleFormatter.
 *
 * @ingroup formatters
 */
class EUtilsBioSampleFormatter extends EUtilsFormatter {

  /**
   * Add the formatted data into a table.
   *
   * @param array $data
   *   The parsed XML data.
   *
   * @return array
   *   Drupal form elements array of each section in a fieldset.
   */
  public function format(array $data) {

    $return = [];

    unset($data['full_ncbi_xml']);

    $header = ['Key', 'Value'];
    $rows = [];

    $rows[] = ['Name', $data['name']];
    $rows[] = ['Description', $data['description']];

    $table = theme('table', ['rows' => $rows, 'header' => $header]);

    $return['base'] = [
      '#type' => 'fieldset',
      '#title' => 'BioSample',
      '#collapsible' => TRUE,
    ];
    $return['base']['table'] = [
      '#markup' => $table,
      '#type' => 'item',
    ];

    $attributes = $data['attributes'];

    $header = ['Key', 'Value'];
    $rows = [];
    $mapper = new TagMapper('biosample');

    foreach ($attributes as $record) {

      $label = $mapper->getDisplayLabel($record);
      if (!$label) {
        tripal_set_message(t('Warning: the property value !value had no label set, and will be ignored.', [
          '!value',
          $record['value'],
        ]), TRIPAL_WARNING);
        continue;
      }
      $row = [$label, $record['value']];

      $rows[] = $row;
    }

    $table = theme('table', ['rows' => $rows, 'header' => $header]);

    $return['attributes'] = [
      '#type' => 'fieldset',
      '#title' => 'Biosample Attributes',
      '#collapsible' => TRUE,
    ];
    $return['attributes']['table'] = [
      '#markup' => $table,
      '#type' => 'item',
    ];

    $accessions = $data['accessions'];

    $header = ['DB', 'Record'];
    $rows = [];

    foreach ($accessions as $record) {
      if (!isset($record['db']) || !isset($record['value'])) {
        continue;
      }
      $accession = $record['value'];
      $link = $this->getDbLink($accession, $record['db']);

      $rows[] = [
        $record['db'] ?? ($record['db_label'] ?? ''),
        $link,
      ];
    }

    $table = theme('table', ['rows' => $rows, 'header' => $header]);

    $return['xref'] = [
      '#type' => 'fieldset',
      '#title' => 'Cross References',
      '#collapsible' => TRUE,
    ];
    $return['xref']['table'] = [
      '#markup' => $table,
      '#type' => 'item',
    ];

    $header = ['Type', 'Accession'];
    $rows = [];
    $accession = $data['organism']['taxonomy_id'];
    $organism_link = $this->getDbLink( $accession, 'organism');

    $rows[] = ['Organism', $organism_link];
    $rows[] = ['Contact', $data['contact']];

    if (!empty($data['projects'])) {
      foreach ($data['projects'] as $project) {
        $link = $this->getDbLink($project, 'bioproject');
        $rows[] = ['Project', $link];
      }
    }

    $table = theme('table', ['rows' => $rows, 'header' => $header]);

    $return['links'] = [
      '#type' => 'fieldset',
      '#title' => 'Additional Records',
      '#collapsible' => TRUE,
    ];
    $return['links']['table'] = [
      '#markup' => $table,
      '#type' => 'item',
    ];
    return $return;

  }

}

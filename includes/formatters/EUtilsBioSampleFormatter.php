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
  public function format(array $data): array {
    $elements = [];

    $header = ['Key', 'Value'];
    $rows = [];
    $rows[] = ['Name', $data['name']];
    $rows[] = ['Description', $data['description']];

    $elements['base'] = [
      '#title' => 'BioSample',
      '#type' => 'details',
      '#collapsible' => TRUE,
      '#open' => TRUE,
    ];
    $elements['base']['table'] = [
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#type' => 'table',
    ];

    $attributes = $data['attributes'];

    $header = ['Key', 'Value'];
    $rows = [];
    // The following is very very slow! @todo
    $mapper = new TagMapper('biosample');

    foreach ($attributes as $record) {

      $label = $mapper->getDisplayLabel($record);
      if (!$label) {
        \Drupal::service('tripal.logger')
          ->warning('Warning: the property value @value had no label set, and will be ignored.',
            ['@value' => $record['value']]);
        continue;
      }
      $row = [$label, $record['value']];

      $rows[] = $row;
    }

    $elements['attributes'] = [
      '#type' => 'details',
      '#title' => 'Biosample Attributes',
      '#collapsible' => TRUE,
      '#open' => TRUE,
    ];
    $elements['attributes']['table'] = [
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
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

    $elements['xref'] = [
      '#type' => 'details',
      '#title' => 'Cross References',
      '#collapsible' => TRUE,
      '#open' => TRUE,
    ];
    $elements['xref']['table'] = [
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
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

    $elements['links'] = [
      '#type' => 'details',
      '#title' => 'Additional Records',
      '#collapsible' => TRUE,
      '#open' => TRUE,
    ];
    $elements['links']['table'] = [
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#type' => 'item',
    ];
    return $elements;
  }
}

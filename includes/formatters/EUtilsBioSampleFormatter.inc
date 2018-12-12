<?php

/**
 * Class EUtilsBioSampleFormatter.
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

    $header = ['DB', 'Value'];
    $rows = [];

    foreach ($accessions as $record) {
      $rows[] = [
        $record['db'] ?? ($record['db_label'] ?? ''),
        $record['value'],
      ];
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

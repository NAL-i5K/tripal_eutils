<?php

/**
 * Class EutilsBioProject Formatter.
 *
 * @ingroup formatters
 */
class EutilsBioProjectFormatter extends EUtilsFormatter {

  private $elements = [];

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

    try {
      $this->formatBaseRecord($data);
      $this->formatAttributes($data['attributes']);
      $this->formatLinkedRecords($data['linked_records']);
    }
    catch (Exception $exception) {
      drupal_set_message($exception->getMessage());
    }

    return $this->elements;
  }

  /**
   * Handle the base Chado.analysis record.
   *
   * @param array $data
   *   From \EutilsAssemblyParser.
   */
  private function formatBaseRecord(array $data) {

    $header = ['Field', 'Value'];
    $accession = $data['accessions']['BioProject'] ?? NULL;

    $rows[] = ['Name', $data['name']];
    $rows[] = ['Description', $data['description']];

    if ($accession) {
      $rows[] = ['Accession', $accession];
    }

    $table = theme('table', ['rows' => $rows, 'header' => $header]);

    $this->elements['base'] = [
      '#type' => 'fieldset',
      '#title' => 'Project',
      '#collapsible' => TRUE,
    ];
    $this->elements['base']['table'] = [
      '#markup' => $table,
      '#type' => 'item',
    ];
  }

  /**
   * Describe the Chado properties that will be associated with this analysis.
   *
   * @param array $attributes
   *   Array of attributes.  Should have stats and file keys.
   */
  private function formatAttributes(array $attributes) {

    if (empty($attributes)) {
      return;
    }
    $rows = [];
    $header = ['Name', 'Value'];

    foreach ($attributes as $key => $value) {
      $rows[] = [$key, $value];
    }

    $table = theme('table', ['rows' => $rows, 'header' => $header]);
    $this->elements['properties'] = [
      '#type' => 'fieldset',
      '#title' => 'Project Properties',
      '#collapsible' => TRUE,
    ];
    $this->elements['properties']['table'] = [
      '#markup' => $table,
      '#type' => 'item',
    ];

  }

  /**
   * Describes additional records that will be created.
   *
   * @param array $accessions
   *   the Accessions array from the EutilsBioProjectParser.
   */
  private function formatLinkedRecords(array $accessions) {

    if (empty($accessions)) {
      return;
    }

    $rows = [];
    $header = ['Type', 'Value'];

    foreach ($accessions as $type => $value) {
      if ($type == 'BioSample') {
        continue;
      }
      $link = $this->getDbLink($value, $type);
      $rows[] = [$type, $link];
    }

    $table = theme('table', ['rows' => $rows, 'header' => $header]);

    $this->elements['links'] = [
      '#type' => 'fieldset',
      '#title' => 'Linked Records',
      '#collapsible' => TRUE,
    ];
    $this->elements['links']['table'] = [
      '#markup' => $table,
      '#type' => 'item',
    ];
  }

}

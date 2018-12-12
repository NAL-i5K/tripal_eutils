<?php

/**
 * Class EutilsBioProject Formatter.
 */
class EutilsBioProjectFormatter extends EUtilsFormatter {

  private  $elements = [];

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
      $this->formatLinkedRecords($data['accessions']);
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

  }

  /**
   * Describe the Chado properties that will be associated with this analysis.
   *
   * @param array $attributes
   *   Array of attributes.  Should have stats and file keys.
   */
  private function formatAttributes(array $attributes) {

  }

  /**
   * Describes additional records that will be created.
   *
   * @param array $accessions
   *   the Accessions array from the EutilsAssemblyParser.
   */
  private function formatLinkedRecords(array $accessions) {

    $rows = [];
    $header = [];

    $table = theme('table', ['rows' => $rows, 'header' => $header]);

    $return[] = [
      '#markup' => $table,
      '#prefix' => t('Additional Records Linked'),
      '#type' => 'item',
    ];
  }
}

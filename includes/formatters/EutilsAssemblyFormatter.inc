<?php

/**
 * Class EUtilsAssembly Formatter.
 */
class EutilsAssemblyFormatter extends EUtilsFormatter {

  private $elements;

  /**
   * Add the formatted data into a table.
   *
   * @param array $data
   *   The parsed XML data.
   * @param array $elements
   *   The array to add Drupal elements to.
   */
  public function format(array $data, array &$elements) {
    $this->elements = $elements;
    try {
      $this->formatBaseRecord($data);
      // unset($data['accessions']['assembly']);.
      $this->formatAttributes($data['attributes']);
      $this->formatLinkedRecords($data);
    }
    catch (Exception $exception) {
      drupal_set_message($exception->getMessage());
    }
  }

  /**
   * Handle the base Chado.analysis record.
   *
   * @param array $data
   *   From \EutilsAssemblyParser.
   */
  private function formatBaseRecord(array $data) {
    $name = $data['name'];
    $description = $data['description'];
    $program = $data['ftp_attributes']['# Assembly method:'];
    $xrefs = $data['accessions']['assembly'];
    $xref_string = implode(', ', $xrefs);
    $header = ['Name', 'Description', 'Program', 'Accessions'];
    $rows = [[$name, $description, $program, $xref_string]];
    $table = theme('table', ['rows' => $rows, 'header' => $header]);

    $this->elements['base_record'] = [
      '#type' => 'fieldset',
      '#title' => 'Analysis Record',
      '#collapsible' => TRUE,
    ];
    $this->elements['base_record']['table'] = [
      '#markup' => $table,
      '#type' => 'item',
    ];
  }

  /**
   *
   */
  private function formatAttributes() {

  }

  /**
   *
   */
  private function linkedRecords() {

  }

  /**
   *
   */
  private function run($parsed) {

    $attributes = $parsed['attributes'];

    // Next build table of chado props from various areas.
    $header = ['Property Name', 'Value'];

    $rows = [];
    foreach ($attributes['stats'] as $key => $value) {
      $rows[] = [$key, $value];
    }
    foreach ($attributes['files'] as $key => $value) {
      $rows[] = [$key, $value];
    }

    $table = theme('table', ['rows' => $rows, 'header' => $header]);

    $return[] = [
      '#markup' => $table,
      '#prefix' => t('Analysis Properties'),
      '#type' => 'item',
    ];

    $rows = [];
    $header = ['Accession Type', 'Value'];

    foreach ($parsed['accessions'] as $accession_key => $value) {

      if ($accession_key == 'Taxon_accession') {
        $rows[] = ['Organism', $value];
        continue;
      }

      if (is_string($value)) {

        $rows[] = [ucfirst($accession_key), $value];
        continue;
      }

      foreach ($value as $item) {
        $rows[] = [ucfirst($accession_key), $item];
      }
    }

    $table = theme('table', ['rows' => $rows, 'header' => $header]);

    $return[] = [
      '#markup' => $table,
      '#prefix' => t('Additional Records Linked'),
      '#type' => 'item',
    ];

    return $return;
  }

}

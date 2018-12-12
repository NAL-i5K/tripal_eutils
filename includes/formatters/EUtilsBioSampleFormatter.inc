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
   * @param array $elements
   *   The array to add Drupal elements to.
   *
   * @return void
   *   Does not return anything.
   */
  public function format(array $data, array &$elements) {
    try {
      $return = [];

      foreach ($data as $key => $item) {
        if ($key == 'full_ncbi_xml') {
          continue;
        }

        if (is_string($item)) {
          $return[] = [
            '#type' => 'item',
            '#title' => ucfirst($key),
            '#markup' => $item,
          ];
          continue;
        }

        if ($key === 'attributes') {
          $header = ['Key', 'Value'];
          $rows = [];

          foreach ($item as $record) {
            $rows[] = [$record['harmonized_name'], $record['value']];
          }

          $table = theme('table', ['rows' => $rows, 'header' => $header]);

          $return[] = [
            '#type' => 'item',
            '#markup' => $table,
            '#title' => ucfirst($key),
          ];

          continue;
        }

        if ($key === 'accessions') {
          $header = ['DB', 'Value'];
          $rows = [];

          foreach ($item as $record) {
            if(!isset($record['db']) && !isset($record['db_label'])) {
              dpm($record);
            }
            $rows[] = [$record['db'] ?? ($record['db_label'] ?? ''), $record['value']];
          }

          $table = theme('table', ['rows' => $rows, 'header' => $header]);

          $return[] = [
            '#markup' => $table,
            '#prefix' => ucfirst($key),
            '#type' => 'item',
          ];
          continue;
        }

        foreach ($item as $record) {
          if (is_string($record)) {
            $return[] = [
              '#type' => 'item',
              '#title' => ucfirst($key),
              '#markup' => $record,
            ];
            continue;
          }
        }
      }
    } catch (Exception $exception) {
      drupal_set_message($exception->getMessage());
    }

    $elements = $return;
  }

}

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
    $elements = [];
    // TODO: move function over to class.
    try {
      $elements = tripal_eutils_parse_biomaterial_for_table($data);
    }
    catch (Exception $exception) {
      drupal_set_message($exception->getMessage());
    }
    return $elements;
  }

}

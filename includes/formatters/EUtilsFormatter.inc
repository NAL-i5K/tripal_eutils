<?php

/**
 * EUtilsFormatter Class.
 */
abstract class EUtilsFormatter {

  /**
   * Format a parser's output.
   *
   * @param array $data
   *   The data array returned by a parser.
   * @param array $elements
   *   The elements array to add the formatted data to.
   *
   * @return void
   *   This function does not return anything. It directly manipulates the
   *    elements array.
   */
  abstract public function format(array $data, array &$elements);

}

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
   *
   * @return array
   *   This function does not return anything. It directly manipulates the
   *    elements array.
   */
  abstract public function format(array $data);

  /**
   * Fetch the DB object for an NCBI DB.
   *
   * @param string $db_name
   *   The DB name as passed by the parser.
   *
   * @return bool
   *   Returns a database object or FALSE.
   */
  public function getNCBIDB(string $db_name) {

    $name = "NCBI {$db_name}";

    $db = db_query(
      'SELECT * FROM chado.db WHERE UPPER(name) = :name',
      [':name' => strtoupper($name)]
    )->fetchObject();
    if ($db) {
      return $db;
    }

    $db = db_query(
      'SELECT * FROM chado.db WHERE UPPER(name) = :name',
      [':name' => strtoupper($db_name)]
    )->fetchObject();
    if ($db) {
      return $db;
    }
    return FALSE;
  }

}

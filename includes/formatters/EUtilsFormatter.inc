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


  /**
   * Generates a URL link given a db string and accession.
   *
   * @param string $accession
   *   Accession string.
   * @param string $db
   *   Database lookup string.
   *
   * @return mixed
   *   returns either the accession string, or the accession with a link to the
   *   xref.
   */
  public function getDbLink(string $accession, string $db) {

    $db = $this->getNCBIDB($db);

    if (!$db) {
      return $accession;
    }

    $fake_dbxref = new stdClass();
    $fake_dbxref->accession = $accession;
    $fake_dbxref->db_id = $db;

    $link = l($accession, chado_get_dbxref_url($fake_dbxref));
    return $link;
  }

}

<?php

use Drupal\Core\Url;
use Drupal\Core\Link;

/**
 * EUtilsFormatter Class.
 *
 * @defgroup formatters
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
   * Fetch the DB object for an NCBI DB. Case insensitive.
   *
   * @param string $db_name
   *   The DB name as passed by the parser.
   * @param Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoDbxrefBuddy $dbxref_instance
   *   A chado buddy dbxref instance
   *
   * @return array
   *   Returns an array of Drupal\tripal_chado\ChadoBuddy\ChadoBuddyRecord,
   *   which will be empty if $db_name was not found.
   */
  public function getNCBIDB(string $db_name, $dbxref_instance) {
    $name = "NCBI {$db_name}";

    if (strtolower($db_name) == 'organism') {
      $name = 'NCBITAXON';
    }
    // First try with case sensitive on in case there might be two matches

    $db = $dbxref_instance->getDb(['db.name' => $name], []);
    if (!$db) {
      $db = $dbxref_instance->getDb(['db.name' => $name], ['case_insensitive' => 'name']);
    }
    return $db;
  }

  /**
   * Generates a URL link given a db string and accession.
   *
   * @param string $accession
   *   Accession string.
   * @param string $db_name
   *   Database lookup string.
   *
   * @return mixed
   *   returns either the accession string, or the accession with a link to the
   *   xref.
   */
  public function getDbLink(string $accession, string $db_name) {
    $buddy_service = \Drupal::service('tripal_chado.chado_buddy');
    /** @var Drupal\tripal_chado\Plugin\ChadoBuddy\ChadoDbxrefBuddy **/
    $dbxref_instance = $buddy_service->createInstance('chado_dbxref_buddy', []);

    // Note that getNCBIDB() is not case sensitive
    $db_records = $this->getNCBIDB($db_name, $dbxref_instance);

    if (count($db_records)) {
      $db_records[0]->setValue('dbxref.accession', $accession);
      $url_string = $dbxref_instance->getDbxrefUrl($db_records[0]);
      $link = Link::fromTextAndUrl($accession, Url::fromUri($url_string,
        ['attributes' => ['target' => '_blank']]));
      return $link;
    }
    else {
      return $accession;
    }
  }

}

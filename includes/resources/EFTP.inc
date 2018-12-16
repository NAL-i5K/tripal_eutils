<?php

/**
 * Right now responsible for getting a single value from the Assembly.
 *
 * @group resources
 */
class EFTP {

  /**
   * The FTP URL.
   *
   * @var string
   */
  protected $url;

  /**
   * Contains the content of the downloaded file.
   *
   * @var string
   */
  protected $file;

  /**
   * @param string $url
   */
  public function setURL($url) {

    $this->url = $url;
    $file = file_get_contents($url);
    $this->file = $file;
  }

  /**
   * Find all records of line starting with a specific item.
   *
   * @param $field
   *
   * @return array
   */
  public function getField(string $field) {
    $results = [];

    $file = $this->file;

    $lines = explode("\n", $file);

    foreach ($lines as $line) {

      if (strpos($line, $field) === 0) {

        $string = str_replace($field, '', $line);

        $string = trim($string);

        $results[] = $string;
      }
      // If (!substr($line, 0, 1) === "#") {
      //        //we're done with the header, stop parsing.
      //        continue;
      //      }.
    }

    return $results;
  }

}

<?php

/**
 * Right now responsible for getting a single value from the Assembly.
 *
 * @ingroup resources
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
   * Get the contents of a file at a given URL.
   *
   * @param string $url
   *   The FTP URL.
   *
   * @throws \Exception
   */
  public function setURL($url) {

    $this->url = $url;
    $file = file_get_contents($url);
    if (!$file) {
      throw new Exception('Unable to connect to FTP resource: ' . $url);
    }
    $this->file = $file;
  }

  /**
   * Find all records of line starting with a specific item.
   *
   * @param string $field
   *   The field is the substring to look for at the start of a line.
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

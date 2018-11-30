<?php

class EUtilsBioSampleParser implements EUtilsParserInterface{

  /**
   * Parse the XML into an array.
   *
   * @param \SimpleXMLElement $xml
   *
   * @return array
   */
  public function parse(SimpleXMLElement $xml) {
    $accessions = $this->extractAttributes($xml->BioSample->Ids);

    $data = [
      'name' => $this->getNameFromAccessions($accessions,
        $xml->BioSample->accession),
      'accessions' => $accessions,
      'description' => isset($xml->BioSample->Description->Comment) ? $this->extractParagraphs($xml->BioSample->Description->Comment) : [],
      'attributes' => $this->extractAttributes($xml->BioSample->Attributes),
    ];

    return $data;
  }

  /**
   * Get sample name from the list of accessions..
   *
   * @param array $accessions
   * @param string $default
   *
   * @return string
   */
  public function getNameFromAccessions(
    array $accessions,
    $default = 'Unknown'
  ) {
    foreach ($accessions as $accession) {
      if (isset($accession['db_label']) && trim(strtolower($accession['db_label'])) === 'sample name') {
        return $accession['value'];
      }
    }

    return $default;
  }

  /**
   * Extract paragraphs.
   *
   * @param \SimpleXMLElement $comment
   *
   * @return array
   */
  protected function extractParagraphs(SimpleXMLElement $comment) {
    $paragraphs = [];

    foreach ($comment->children() as $paragraph) {
      $paragraphs[] = (string) $paragraph;
    }

    return $paragraphs;
  }

  /**
   * Extract attributes.
   *
   * @param \SimpleXMLElement $elements
   *
   * @return array
   */
  protected function extractAttributes(SimpleXMLElement $elements) {
    $data = [];
    foreach ($elements->children() as $element) {
      $datum = [
        'value' => (string) $element,
      ];

      foreach ($element->attributes() as $key => $value) {
        $datum[$key] = (string) $value;
      }

      $data[] = $datum;
    }

    return $data;
  }
}

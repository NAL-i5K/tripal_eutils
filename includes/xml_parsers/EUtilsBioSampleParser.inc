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
    $data = [
      'name' => (string) $xml->BioSample->Description->Title,
      'accession' => (string) $xml->BioSample['accession'],
      'description' => isset($xml->BioSample->Description->Comment) ? $this->extractParagraphs($xml->BioSample->Description->Comment) : NULL,
      'attributes' => $this->extractAttributes($xml->BioSample->Attributes),
    ];

    return $data;
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
        $datum[$key] = (string)$value;
      }

      $data[] = $datum;
    }

    return $data;
  }
}

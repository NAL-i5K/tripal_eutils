<?php

class EUtilsBioSampleParser implements EUtilsParserInterface {

  /**
   * Parse the XML into an array.
   *
   * @param \SimpleXMLElement $xml
   *
   * @return array
   */
  public function parse(SimpleXMLElement $xml) {
    // Primary data
    $primary_accession = (string) $xml->BioSample['accession'];
    $description       = $xml->BioSample->Description->Comment ??
      $this->extractParagraphs($xml->BioSample->Description->Comment);

    // Get accessions
    $accessions = $this->extractAttributes($xml->BioSample->Ids);

    // Get properties
    $attributes = $this->extractAttributes($xml->BioSample->Attributes);
    // Check and extract owner given db_label
    $this->attachDBLabel($attributes, $accessions);

    return [
      'name'          => $primary_accession,
      'accessions'    => $accessions,
      'description'   => $description,
      'attributes'    => $attributes,
      'contact'       => $this->extractContact($xml->BioSample),
      'organism'      => $this->extractOrganism($xml->BioSample),
      'full_ncbi_xml' => $xml->BioSample->asXML(),
    ];
  }

  /**
   * Extract the db_label from accessions and add it to attributes if db_label
   * is Sample name.
   *
   * @param array $attributes
   * @param array $accessions
   */
  protected function attachDBLabel(&$attributes, $accessions) {
    $accessions = array_filter(
      $accessions, function ($accession) {
      return isset($accession['db_label'])
        && strtolower($accession['db_label']) === 'sample name';
    }
    );

    foreach ($accessions as $accession) {
      $attributes[] = [
        'value'           => $accession['value'],
        'harmonized_name' => 'submitter_provided_accession',
        'attribute_name'  => 'submitter_provided_accession',
        'display_name'    => $accession['db_label'],
      ];
    }
  }

  /**
   * Get the organism.
   *
   * @param \SimpleXMLElement $xml
   *
   * @return array The organism data [ taxonomy_name, taxonomy_id, value ]
   */
  protected function extractOrganism($xml) {
    if (!isset($xml->Description->Organism)) {
      return NULL;
    }

    $organism = $xml->Description->Organism;

    return (array) $organism->attributes() + ['value' => (string) $organism];
  }

  /**
   * Get the owner name.
   *
   * @param \SimpleXMLElement $xml
   *
   * @return string|NUll
   */
  protected function extractContact($xml) {
    return $xml->Owner->Name ?? NULL;
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

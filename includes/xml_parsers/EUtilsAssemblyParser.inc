<?php

/**
 * Parser for NCBI Assembly https://www.ncbi.nlm.nih.gov/assembly/ XML.
 */
class EUtilsAssemblyParser implements EUtilsParserInterface {

  /**
   * @param \SimpleXMLElement $xml
   *
   * @return array
   */
  public function parse(SimpleXMLElement $xml) {
    $info = [
      'attributes' => [],
    ];

    // Skip to DocumentSummary.
    $xml = $xml->DocumentSummarySet;
    $xml = $xml->DocumentSummary;

    $children = $xml->children();

    $info['full_ncbi_xml'] = $xml->asXML();

    foreach ($children as $key => $child) {
      switch ($key) {
        case 'Meta':
          $info['attributes'] = $this->parseMeta($child);

          break;

        case 'AssemblyName':
          $info['name'] = (string) $child;
          break;

        case 'AssemblyDescription':
          $info['description'] = (string) $child;
          break;

        case 'AssemblyAccession':
          $info['ignored'][] = $child->getName();
          break;

        // Linked chado records.
        case 'SpeciesTaxid':
          // NOTE: Taxid also exists, but in all examples they are equal.
          $info['accessions']['taxon_accession'] = (string) $child;
          break;

        case 'RsUid':
          $info['accessions']['assembly']['Refseq'] = (string) $child;
          break;

        case 'GbUid':
          $info['accessions']['assembly']['Genbank'] = (string) $child;
          break;

        case 'WGS':
          $info['accessions']['assembly']['WGS'] = (string) $child;

        case 'GB_BioProjects':
        case 'RS_BioProjects':

          $bioproject_children = $child->children();

          foreach ($bioproject_children as $bp_child) {
            $id = $bp_child->BioprojectId;
            // TODO: do we also need to keep track of if this is genbank or refseq?
            $info['accessions']['bioprojects'][] = (string) $id;
          }

          break;

        case 'BioSampleId':
          $info['accessions']['biosamples'][] = (string) $child;
          break;

        case 'SubmissionDate':
          $info['submission_date'] = (string) $child;

        default:
          $info['ignored'][] = $child->getName();
          break;
      }
    }

    return $info;
  }

  /**
   * Parse the <Meta> tag, which containts CDATA but all of the Assembly props.
   *
   * @param $x
   *
   * @return array
   */
  public function parseMeta($x) {
    $list = [];
    $trimmed = trim((string) $x);

    // Wrap this in a root in case its malformed.
    $wrapped = '<wrap>' . $trimmed . '</wrap>';
    $meta = simplexml_load_string($wrapped);

    $children = $meta->children();
    foreach ($children as $key => $child) {
      switch ($key) {
        case 'Stats':
          $list['stats'] = $this->processFinalChildren($child, [
            'category',
            'sequence_tag',
          ]);
          break;

        case 'FtpSites':
          $list['files'] = $this->processFinalChildren($child, ['type']);
          break;

        default:
          break;
      }
    }

    $url = $list['files']['Assembly_rpt'] ?? NULL;

    if ($url) {
      $list['ftp_attributes'] = $this->getFTPData($url);
    }

    return $list;
  }

  /**
   * Process terminal level children (IE a flat array of children).
   *
   * @param $x
   *   - xml of children
   * @param $keys
   *   - attribute keys to derive the label from
   *
   * @return array
   */
  private function processFinalChildren($x, $keys) {
    $results = [];
    $children = $x->children();

    foreach ($children as $child) {
      $attributes = $child->attributes();

      $label = "";
      foreach ($keys as $key) {
        $label .= $attributes->$key;
        // If this isn't the last key in the array, add an underscore.
        if (next($keys)) {
          $label .= '_';
        }
      }

      $results[$label] = (string) $child;
    }

    return $results;
  }

  /**
   * Get the fields the assembly object will need from the FTP.
   *
   * @param $url
   *   - the ftp site url extracted form the metadata
   *
   * @return array
   */
  public function getFTPData($url) {

    $ftp = new EFTP();

    $ftp->setURL($url);

    $data = [];

    $fields = ['# Assembly method:'];

    foreach ($fields as $field) {
      $values = $ftp->getField($field);

      if (count($values === 0)) {

        $value = $values[0];
      }
      else {
        $value = implode('', $values);
      }

      $data[$field] = $value;
    }

    return $data;
  }

}

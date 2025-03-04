<?php

/**
 * Class EUtilsBioProjectParser.
 *
 * Note that projects don't have reliable attribute listings.
 *
 * @ingroup parsers
 */
class EUtilsBioProjectParser implements EUtilsParserInterface {

  /**
   * Parse an NCBI BioProject XML.
   *
   * @param \SimpleXMLElement $xml
   *   Simple XML Element.
   *
   * @return array|mixed
   *   Array.
   *
   * @throws \Exception
   */
  public function parse(SimpleXMLElement $xml) {
    $info = [
      'name' => '',
      'description' => '',
      'accessions' => [],
      'attributes' => [],
      // Expected keys for linked records:  Contact, biomaterial, analysis, organism.
      'linked_records' => [],
      'full_ncbi_xml' => '',
    ];

    // Jump to Project node.
    $projects = $xml->xpath('DocumentSummary/Project');
    if (!isset($projects[0])) {
      throw new Exception('Unexpected XML structure');
    }

    $project = $projects[0];
    $info['full_ncbi_xml'] = $project->asXML();

    $children = $project->children();
    foreach ($children as $key => $child) {
      switch ($key) {

        case 'ProjectID':
          // Accession for the project.
          $primary_xref = $child->ArchiveID;
          $id = $primary_xref->attributes()->id;
          $info['accessions']['BioProject'] = (string) $id;
          break;

        case 'ProjectDescr':

          // Information about the project itself.  Includes title, description.
          $name_string = implode(': ', array_filter([(string) $child->Name, (string) $child->Title]));
          $info['name'] = $name_string;
          $info['description'] = (string) $child->Description;

          // LocusTagPrefix (can be multiple) has biosample and assembly.
          if ($child->LocusTagPrefix) {
            $gchildren = $child->children();

            foreach ($gchildren as $key => $gchild) {
              if ($key != 'LocusTagPrefix') {
                continue;
              }
              $attributes = $gchild->attributes();

              foreach ($attributes as $key => $value) {
                // Key is biosample_id, assembly_id.  remove the _id suffix.
                $key = str_replace('_id', '', $key);
                $info['linked_records'][$key][] = (string) $value;
              }
            }
          }

          // Publications.
          $pubs = $this->extractPubs($child);
          $info['linked_records']['pubs'] = $pubs;

          break;

        case 'ProjectType':
          // Includes organism, metadata for project.
          $target = $child->ProjectTypeSubmission->Target;

          if (!$target) {
            break;
          }

          $organism = $target->Organism;
          $organism_accession = NULL;

          if ($organism) {
            // Extract organism info.
            $attributes = $organism->attributes();

            foreach ($attributes as $attribute_key => $value) {

              if ($attribute_key == 'taxID') {
                $organism_accession = $value;

              }
              // taxID, species are both ncbi ID.  taxID is more precise.
            }

            $info['linked_records']['organism'] = (string) $organism_accession;
          }

          break;

        case 'ProjectID':
          // Accession info for the project.  Should match what was submitted, thats about it.
          break;

        default:
          throw new Exception('Unknown tag ' . $key);
      }
    }

    return $info;
  }

  /**
   * @param \SimpleXMLElement $xml
   *
   * @return array
   */
  public function bioProjectSubmission(SimpleXMLElement $xml) {
    $info = [];

    // First get attributes of parent.
    $attributes = $xml->attributes();

    // For example:
    // ["last_update"]=>
    //    string(10) "2018-11-21"
    //    ["submission_id"]=>
    //    string(10) "SUB4827559"
    //    ["submitted"]=>
    //    string(10) "2018-11-21".
    // Now deal with children.
    $children = $xml->children();

    foreach ($children as $key => $child) {

      // Keys so far are description and action.  No one cares about action?
      if ($key == 'Description') {
        $org = $child->Organization->Name;
        $info['organization'] = (string) $org;
      }
    }
    return $info;
  }

  /**
   * Get publication IDs.  Note we assume these are always pubmed IDs.
   *
   * @param \SimpleXMLElement $xml
   *   ProjectDscr XML tag.
   *
   * @return array
   *   An array of pub IDs.
   */
  private function extractPubs(\SimpleXMLElement $xml) {
    $pub_out = [];

    $pubs = $xml->xpath("//Publication");

    foreach ($pubs as $pub) {

      $attributes = $pub->attributes();

      foreach ($attributes as $key => $val) {
        if ($key == 'id') {
          $pub_out[] = (string) $val;
        }
      }

    }
    return $pub_out;
  }

}

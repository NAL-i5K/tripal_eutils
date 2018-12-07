<?php

/**
 * Class EUtilsBioProjectParser
 *
 * Note that projects dont have reliable attribute listings.
 */
class EUtilsBioProjectParser implements EUtilsParserInterface {

  /**
   * @param \SimpleXMLElement $xml
   *
   * @return array|mixed
   * @throws \Exception
   */
  public function parse(SimpleXMLElement $xml) {
    $info = [
      'name'           => '',
      'description'    => '',
      'accessions'     => [],
      'attributes'     => [],
      //Expected keys for linked records:  Contact, biomaterial, analysis, organism
      'linked_records' => [],
      'full_ncbi_xml'  => '',
    ];

    //Jump to Project node
    $projects = $xml->xpath('DocumentSummary/Project');
    $project  = $projects[0];

    if (!$project) {

      $e = 'Unexpected XML structure';

      throwException($e);
    }
    $info['full_ncbi_xml'] = $project->asXML();

    $children = $project->children();
    foreach ($children as $key => $child) {
      switch ($key) {

        case 'ProjectID':
          //Accession for the project.
          $primary_xref = $child->ArchiveID;
          break;

        case 'ProjectDescr':
          //Information about the project itself.  Includes title, description

          $name_string         = (string) $child->Name . ': '
                                 . (string) $child->Title;
          $info['name']        = $name_string;
          $info['description'] = (string) $child->Description;

          //do we have a LocusTagPrefix?  If so that links to biosample and/or assembly.
          //

          if ($child->LocusTagPrefix) {

            $attributes = $child->attributes();

            $x = [];

            foreach ($attributes as $key => $value) {


              $x[(string) $key] = (string) $value;
            }

            $info['linked_records']['locus_tag_prefix']['attributes'] = $x;
            $info['linked_records']['locus_tag_prefix']['value']
                                                                      = (string) $child;
          }

          break;

        case 'ProjectType':
          //Includes organism, metadata for project.

          $target = $child->ProjectTypeSubmission->Target;

          if (!$target) {
            break;
          }

          $organism        = $target->Organism;
          $organism_record = [];

          if ($organism) {
            //Extract organism info

            $attributes = $organism->attributes();

            foreach ($attributes as $attribute_key => $value) {

              $organism_record['attributes'][$attribute_key] = $value;
              //taxID, species are both ncbi ID- dont want to accidentally think that number is the organism.species value!

            }

            $organism_children = $organism->children();

            foreach ($organism_children as $child => $value) {

              $organism_record[(string) $child] = (string) $value;
            }

            //put complete organism record in $info
            $info['linked_records']['organism'] = $organism_record;
          }

          //What about other children and their attributes?

          //          $info['type']['organism']['taxID'] = $attributes['taxID'];

          break;

        case 'ProjectID':
          //Accession info for the project.  Should match what was submitted, thats about it.

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

    //First get attributes of parent.

    $attributes = $xml->attributes();

    //for example:
    // ["last_update"]=>
    //    string(10) "2018-11-21"
    //    ["submission_id"]=>
    //    string(10) "SUB4827559"
    //    ["submitted"]=>
    //    string(10) "2018-11-21"

    //Now deal with children

    $children = $xml->children();

    foreach ($children as $key => $child) {

      //Keys so far are description and action.  No one cares about action?
      if ($key == 'Description') {
        $org                  = $child->Organization->Name;
        $info['organization'] = (string) $org;
      }
    }
    return $info;
  }
}

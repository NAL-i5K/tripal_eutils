<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

module_load_include('inc', 'tripal_eutils', 'includes/Euitils');


class AssembleExamples extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */


  public function testGenerateXMLs() {

    $lookup = [

      'bioproject' => [

      ],
      'biosample' => [

      ],
      'assembly' => [
        '317138',#dog
        '751381', #rubber
          '91111',    #locust
       '1949871', #honeybee
        '2004951',#hemp
       '559011' ,#regia
        '524058',#yak
        '557018'#strawberry
      ],

    ];
    $connection = new \Euitils();

    foreach ($lookup as $db => $accessions) {
      $connection->set_db($db);
      foreach ($accessions as $accession) {
        $result = $connection->lookup_accessions([$accession]);
        $x = simplexml_import_dom($result);
        $x->asXML($accession . '_' . $db . '.xml');
        sleep(.3);
      }
    }
  }

}

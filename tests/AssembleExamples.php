<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

module_load_include('inc', 'tripal_eutils', 'includes/resources/Eutils');


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
        '185471',
        '13179',
        '12384',
        '221623',
        '60037',#strawberry
        '66853',#strawberry
        '291087',
        '350852',
        '310386',
        '394253',
        '471592',
        '477511',
        '73819',
      ],
      'biosample' => [
        '2261463',
        '744358', #yak
        '120060',
        '3704235',
        '4451765',
        '9259743',
        '2981385',
        '2953603'
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
    $connection = new \EUtils();

    foreach ($lookup as $db => $accessions) {
      foreach ($accessions as $accession) {
        $result = $connection->lookupAccessions($db, [$accession]);
        $result->asXML($accession . '_' . $db . '.xml');
        sleep(.3);
      }
    }
  }

}

<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class BiosamplePropertyLookupTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  /**
   * @group propertylookup
   */
  public function testBiosamplePropertyLookup() {


    $lookup = new \BiosamplePropertyLookup();

    $terms  = $lookup->lookupAll();

    $this->assertNotEmpty($terms);


    //Version 1.0 as of November 29 2018: 456 terms.
    
    $this->assertEquals('456', count($terms));


  }
}

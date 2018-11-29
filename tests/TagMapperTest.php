<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class TagMapperTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;


  /**
   * @group attributes
   */
  public function testClassInits() {

    $mapper = new \TagMapper('biosample');

   $term =  $mapper->lookup('sex');

   $this->assertNotNull($term);
  }


}

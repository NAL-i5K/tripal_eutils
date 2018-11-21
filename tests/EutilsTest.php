<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
module_load_include('inc', 'tripal_eutils', 'includes/Euitils');


class EutilsTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  //use DBTransaction;




  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testBasicExample() {
    $connection = new \Euitils();
    $count = $connection->check_status();
    $this->assertNotFalse($count);
  }
}

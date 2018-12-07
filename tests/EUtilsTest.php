<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EUtilsTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;

  public function testSettingDB() {
    $connection = new \EUtils();
    $this->expectException('Exception');
    $connection->get('waffles', ['000000']);
  }

}

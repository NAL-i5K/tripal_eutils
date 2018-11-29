<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EFTPTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  // use DBTransaction;

  /**
   *
   * @group ftp
   */
  public function testFTPFindsProgram() {

    $ftp = new \EFTP();
$url = 'ftp://ftp.ncbi.nlm.nih.gov/genomes/all/GCF/000/002/285/GCF_000002285.3_CanFam3.1/GCF_000002285.3_CanFam3.1_assembly_report.txt';
    $ftp->setURL($url);

    $result = $ftp->getField('# Assembly method:');

    $this->assertNotEmpty($result);

  }
}

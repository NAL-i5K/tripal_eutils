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
   * this test works local, but not on travis.
   */
  public function testFTPFindsProgram() {

    if(getenv('IS_TRAVIS')){
      $this->assertTrue(TRUE);
    return;
    }

    $ftp = new \EFTP();
$url = 'ftp://ftp.ncbi.nlm.nih.gov/genomes/all/GCF/000/002/285/GCF_000002285.3_CanFam3.1/GCF_000002285.3_CanFam3.1_assembly_report.txt';
    $ftp->setURL($url);

    $result = $ftp->getField('# Assembly method:');

    $this->assertNotEmpty($result);
  }

  /**
   * @group ftp
   *
   * Unlike above, we mock the assembly report file.  We do this because Travis cant download from the FTP probably due to something like this: https://github.com/travis-ci/travis-ci/issues/3692#issuecomment-94563194
   */


  public function testParseFTP(){

    $ftp = new \EFTP();
    $url = __DIR__ . '/../examples/assembly/ftp/317138_assembly_stats_report.txt';
        $ftp->setURL($url);

        $result = $ftp->getField('# Assembly method:');

        $this->assertNotEmpty($result);

  }
}

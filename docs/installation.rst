Installation and Setup
=======================

Requirements
------------

Tripal EUtilities requires:

- Tripal 3
- PHP >= 7.0
- Drupal 7

Installation
------------

``tripal_eutils`` is not available for deployment via Drush and must be installed via git.

.. code-block:: shell

  cd [location of your custom or contrib modules]
  git clone https://github.com/NAL-i5K/tripal_eutils.git
  drush pm-enable tripal_eutils -y


Chado
-----

This module requires Chado 1.3 or greater.  Visit ``/admin/tripal/storage/chado/install`` on your site to verify and/or upgrade your Chado version.

Setup
-----
This module currently functions "as is" without setup.  The Manage Analyses module provides several new fields (analysis and organism linker fields) so you should **Check For New Fields** on the content types your site utilizes that have _organism or _analysis Chado linker tables.

Additional module-wide settings can be configured at: ``/admin/tripal/tripal_eutils``.

.. image:: /_static/settings_example.png


NCBI API Key
~~~~~~~~~~~~

To get the most out of this module, we suggest setting up an NCBI API key for your site.

NCBI limits requests to a maximum of three/second.  If you use this module to import linked records, you may exceed that, and might benefit from adding an API key.
`This NCBI blog post <https://ncbiinsights.ncbi.nlm.nih.gov/2017/11/02/new-api-keys-for-the-e-utilities/>`_ details the reasoning behind their policy, and provides instructions for getting a key.


Permissions
~~~~~~~~~~~~

This module only defines one permission: ``access tripal_eutils admin``.  This permission will allow users to use the admin form to directly insert Chado records into the database given NCBI accessions.  Because this form adds data to your db, we suggest reserving it for administrators.


Updating
~~~~~~~~~~~~
As of database update 7303, the database and controlled vocabulary for terms used by this module have been converted from ncbi_properties to NCBI Biosample Attributes.
This was done to match the Tripal Biomaterial module's schema, which also overhauled its terminology, coincidentally also in database update 7303.
Instructions for updating to the new schema can be found in the `Tripal Biomaterial module <https://github.com/dsenalik/tripal_analysis_expression/>`_. There is a "Module Updating" section that applies to both of these modules.

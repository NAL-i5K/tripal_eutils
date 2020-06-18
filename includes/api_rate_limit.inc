<?php

/**
 * Utility/timer functions for API Rate Limiting 
 */

/**
 * Utility function to get the SSE and microseconds
 */
function get_mtime() {
    $right_now = DateTime::createFromFormat('U.u', microtime(true));
    // Occasionally calling DateTime will fail, keep trying until it doesn't.
    while(!$right_now)
    {
        usleep(50000);
        $right_now = DateTime::createFromFormat('U.u', microtime(true));
    }
    return floatval($right_now->format("U.u")); 
}

/**
 * Adds an entry to the global variable that an NCBI request has been made
 */
function tripal_eutils_register_access() {
    $last_access = variable_get('tripal_eutils_api_timer',NULL);

    $last_access = get_mtime();

    variable_set('tripal_eutils_api_timer',$last_access);
}

/**
 * Checks that EUtils can continue making requests to NCBI without
 * exceeding the API rate limits
 * 
 * @return boolean
 *  TRUE if EUtils has to wait, false otherwise
 *    Enough time has passed or no recent requests made
 */
function api_timer_wait() {
    $last_access = variable_get('tripal_eutils_api_timer',NULL);
    // Determine the rate limit
    if (variable_get('tripal_eutils_ncbi_api_key'))
    {
        // API Key exists, was checked at previous check. Full speed
        $limit = 10;
    }
    else {
        // No API key exists. Rate limit to 3 per second 
        $limit = 3;
    }

    // No known previous access, go for it
    if (!$last_access) {
        return false;
    }
    else {
        // Determine appropriate time to wait (+ a little padding, NCBI is sensitive)
        $wait_time = 1.0/$limit + 0.2;

        // Current time (format: seconds since epoch.microseconds)
        $now = get_mtime();

        $variance = number_format($now - $last_access, 5);
        if ($variance > $wait_time) {
            // Don't wait
            echo "Accessing...\n";
            return false;
        }
        else {
            // Gotta wait
            echo "Waiting...\n";
            return true;
        }
    }
}
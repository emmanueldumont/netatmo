#!/usr/bin/php
<?php
/**
* Example of Weather station API
* If you need more details, please take a glance at https://dev.netatmo.com/doc
*/

define('__ROOT__', dirname(dirname(__FILE__)));
require_once (__ROOT__.'/src/Netatmo/autoload.php');
require_once 'Utils.php';
require_once 'Config.php';



/**
 * Prints a list of devices
 *
 */
function printDevices($devices, $title = NULL)
{
    if(!is_null($devices) && is_array($devices) && !empty($devices))
    {
        if(!is_null($title))
            printMessageWithBorder($title);

        foreach($devices as $device)
        {
            printWSBasicInfo($device);
        }
    }
}



//App client configuration
$scope = Netatmo\Common\NAScopes::SCOPE_READ_STATION;
$config = array("client_id" => $client_id,
                "client_secret" => $client_secret,
                "username" => $test_username,
                "password" => $test_password);

$client = new Netatmo\Clients\NAWSApiClient($config);

//Authentication with Netatmo server (OAuth2)
try
{
    $tokens = $client->getAccessToken();
}
catch(Netatmo\Exceptions\NAClientException $ex)
{
    handleError("An error happened while trying to retrieve your tokens: " .$ex->getMessage()."\n", TRUE);
}

//Retrieve user's Weather Stations Information

try
{
    //retrieve all stations belonging to the user, and also his favorite ones
    $data = $client->getData(NULL, TRUE);
}
catch(Netatmo\Exceptions\NAClientException $ex)
{
    handleError("An error occured while retrieving data: ". $ex->getMessage()."\n", TRUE);
}

if(empty($data['devices']))
{
    echo 'No devices affiliated to user';
}
else
{

    $users = array();
    $friends = array();
    $fav = array();
    $device = $data['devices'][0];
    $tz = isset($device['place']['timezone']) ? $device['place']['timezone'] : "GMT";

    //devices are already sorted in the following way: first weather stations owned by user, then "friend" WS, and finally favorites stations. Still let's store them in different arrays according to their type
    foreach($data['devices'] as $device)
    {

        //favorites have both "favorite" and "read_only" flag set to true, whereas friends only have read_only
        if(isset($device['favorite']) && $device['favorite'])
            $fav[] = $device;
        else if(isset($device['read_only']) && $device['read_only'])
            $friends[] = $device;
        else $users[] = $device;
    }

    //print first User's device Then friends, then favorite
    printDevices($users, NULL);
}
?>

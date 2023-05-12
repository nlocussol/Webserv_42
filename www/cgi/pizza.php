 <?php
if (isset($_GET['source'])) { die(highlight_file(__FILE__, 1)); }
//  version PHP de pizza3.cgi Chapitre 7 de 
//   Web Applications: Concepts & Real World Design
//   by Craig D. Knuckles, David S. Yuen, wiley, February 2004,
//        http://www.cknuckles.com/webapps/chap07/chap7.html

$dataDir = "";  // data in same directory as this program
$SCRIPT_NAME=$_SERVER['SCRIPT_NAME']; 

//### the data source #######################################
$sizes = array("large", "medium", "small");
$default_size="medium";

$size_prices = array(
  "large" => "8.00",
  "medium" => "6.00",
  "small" => "4.00"
);
$topping_prices = array(
  "large" => "1.00",
  "medium" => "0.75",
  "small" => "0.50"
);
$toppings = array(
  "m_pepperoni" => "Pepperoni",
  "m_sausage" => "Italian Sausage",
  "v_peppers" => "Green Bell Peppers",
  "v_mushrooms" => "Mushrooms",
  "v_onions" => "Vidallia Onions",
  "v_olives" => "Black Olives"
);
//### end of data source ####################################

function print_form(){
    global $sizes,$default_size,$size_prices,$topping_prices,$toppings,$SCRIPT_NAME;
 print<<<TOP
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
   <head>
       <meta http-equiv="content-type" content="text/html;charset=utf-8"/>
       <title>Personalize a pizza</title>
   </head>
   <body>
       <form action="$SCRIPT_NAME" method="get" >
       <table border="1">
            <tr><th>Select size</th><th>Select toppings</th></tr>
            <tr><td>
TOP;

 foreach ($sizes as $size){
   print '<input type="radio" name="chosen_size" value="'.$size.'"';

   if($size == $default_size) {
     print ' checked="checked"';
   }

   print "/><b>$size</b><br/>Base Price: \$$size_prices[$size] <br/>".
     "Each topping: \$$topping_prices[$size]<br/>\n";
 }
 print " </td><td>\n";

 foreach ($toppings as $topping => $value) {
  print '<input type="checkbox" name="'.$topping.'" value="yes"/>'.
    "$toppings[$topping]<br/>\n";
 }
 
 print<<<BOTTOM
    </td></tr>
   </table>
   <p><input type="hidden" name="request" value="confirmation_page"/>
   <input type="submit" value="Calculate total"/></p>
   </form>
  </body>
 </html>
BOTTOM;
}

//#################################################################
function confirmation_page(){
    global $sizes,$default_size,$size_prices,$topping_prices,$toppings,$SCRIPT_NAME;
 print <<<TOP
 <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
 <html xmlns="http://www.w3.org/1999/xhtml">
  <head>
   <meta http-equiv="content-type" content="text/html;charset=utf-8"/>
   <title>Calculating cost of your pizza</title></head>
   <body>
TOP;

 $size = $_GET["chosen_size"];
 $total=$size_prices[$size];
 print "You have selected a <b>$size</b> pizza with the following extra toppings:\n<ul>";

 ksort($toppings);
 foreach ($toppings as $topping=>$value) {
  if(array_key_exists($topping,$_GET)) {
    $total = $total + $topping_prices[$size];
    print "<li><b>$value</b></li>\n";
  }
 }
 print "</ul>\n";

 $total = sprintf("%.2f", $total);
 print "for a total of <b>\$$total</b>.\n";

 print<<<CONFIRM
   Please give the following information to confirm order.<br/>
   <form action="$SCRIPT_NAME" method="get">
    Name: <input type="text" name="name" value="" size="20"/><br/>
    Phone: <input type="text" name="phone" value="" size="20"/><br/>
    <input type="hidden" name="chosen_size" value="$size"/>
    <input type="hidden" name="total" value="$total"/>
    <input type="hidden" name="request" value="confirm_order"/>
CONFIRM;

 foreach ($toppings as $topping=>$value) {
  if(isset($_GET[$topping]) and $_GET[$topping]=="yes") {
    print '   <input type="hidden" name="'.$topping.'" value="yes"/>'."\n";
  }
 }

 print<<<BOTTOM
    <input type="submit" value="Confirm Order"/>
   </form>
   </body>
  </html>
BOTTOM;
}

#################################################################
function confirm_order(){
    global $sizes,$default_size,$topping_prices,$toppings,$dataDir;

   $size = $_GET["chosen_size"];
   $total= $_GET["total"];

   if(!($orderFile = fopen("order3.txt","a")))
      errorPage();
  fwrite($orderFile,"name=".$_GET["name"]."\n");
  fwrite($orderFile,"phone=".$_GET["phone"]."\n");
  fwrite($orderFile,"total=".$total."\n");
  fwrite($orderFile,"size=".$size."\n");
  ksort($toppings);
  foreach($toppings as $topping) {
      if(array_key_exists($topping,$_GET)) {
        fwrite($orderFile,$toppings[$topping]."\n");
      }
  }
 fwrite($orderFile,"---------------------------------------\n");
 fclose($orderFile);
 
 print <<<TOP
 <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
 <html xmlns="http://www.w3.org/1999/xhtml">
  <head>
   <meta http-equiv="content-type" content="text/html;charset=utf-8"/>
   <title>Thank you for your order</title></head>
   <body>
    Thank you {$_GET["name"]} (phone {$_GET["phone"]}).
       We have processed your order of a <b>$size</b> pizza
       with the following extra toppings:<ul>
TOP;
 foreach ($toppings as $topping=>$value) {
  if(array_key_exists($topping,$_GET)) {
    print "<li><b>$value</b></li>\n";
  }
 }

 print<<<BOTTOM
      </ul>
 for a total of <b>\$$total</b>.
   </body>
  </html>
BOTTOM;
}

#################################################################
function errorPage($message) {
   print<<<ALL
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
     "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
   <meta http-equiv="content-type" content="text/html;charset=utf-8"/>
   <title>Server Error</title><
  </head>
  <body>
    <h2>Server Error Encountered</h2>
    $message 
    If the problem persists, please notify the <a href="mailto:admin\@uweb.edu">webmaster</a>.
  </body>
</html>
ALL;
 exit;   # terminate program since failure to open data file
}

//### app logic #############################################
if (isset($_GET["request"])){
    if($_GET["request"] == "confirmation_page") { confirmation_page(); }
    else if($_GET["request"] == "confirm_order") { confirm_order(); }
} else {
    print_form();
}


 <?php
if (isset($_GET['source'])) { die(highlight_file(__FILE__, 1)); }
?>
<?php
//  version PHP de food2.cgi Chapitre 7 de 
//   Web Applications: Concepts & Real World Design
//   by Craig D. Knuckles, David S. Yuen, wiley, February 2004,
//        http://www.cknuckles.com/webapps/chap07/chap7.html

// base de données des prix
$food_prices = array("Cheeseburger" => "1.50",
                     "Veggieburger" => "1.50",
                     "Fries" => "1.00",
                     "Drink" => "0.80" );
$SCRIPT_NAME=$_SERVER['SCRIPT_NAME']; 

function printForm(){
    global $food_prices,$SCRIPT_NAME;
    print <<<TOP
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
 <head>
  <meta http-equiv="content-type" content="text/html;charset=utf-8"/>
  <title>Order some food</title>
 </head>
 <body>
  <form action="$SCRIPT_NAME" method="get">
  <table border="1">
   <tr><th>Food</th><th>Quantity</th></tr>
TOP;

  foreach ($food_prices as $key=>$value)
  print<<<ITEM
    <tr>
    <td>$key (\$$value)</td>
    <td><input type="text" name="$key" value="" size="5"/></td>
   </tr>
ITEM;

  print<<<BOTTOM
  </table>
  <p><input type="submit" value="Submit order"/></p>
  </form>
 </body>
</html>
BOTTOM;
}

function printResults(){
   global $food_prices;
   print <<<TOP
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
 <head>
  <meta http-equiv="content-type" content="text/html;charset=utf-8"/>
  <title>Thank you for your order</title>
</head>
<body>

  <table border="1">
   <tr><th>Food</th><th>Quantity</th><th>Subtotal</th></tr>
TOP;

$total=0;
foreach ($food_prices as $key=>$value){
  $subtotal = sprintf("%.2f", $food_prices[$key]*$_GET[$key]);
  $total += $subtotal;
  print<<<ITEM
    <tr>
    <td>$key (\$$food_prices[$key])</td>
    <td align="right">$_GET[$key]</td><td align="right">\$$subtotal</td>
   </tr>
ITEM;
}

$total = sprintf("%.2f", $total);
print <<<BOTTOM
    <tr>
    <td colspan="2"><b>TOTAL</b></td><td align="right">\$$total</td>
   </tr>
  </table>

</body>
</html>
BOTTOM;
}

// logique de l'application
if(count($_GET)==0)
    printForm();
else
    printResults();
?> 

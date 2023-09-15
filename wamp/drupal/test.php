<?php

/*
$xmlstr = <<< XML
<?xml version='1.0'?>
<annotations><mistake id="m1248957964381"
start="/0.27"
end="/0.30"><type>Vt</type><correction>is</correction><comments/></mistake><mistake
id="m1248958021390" start="/0.88"
end="/0.97"><type>Vt</type><correction>perceive</correction><comments/></mistake><mistake
id="m1248958043607" start="/0.222" end="/0.226"><type>Mec</type><correction>.
667</correction><comments/></mistake><mistake id="m1248958061967" start="/0.271"
end="/0.275"><type>Vform</type><correction/><comments>delete</comments></mistake><mistake
id="m1248958123278" start="/0.318"
end="/0.329"><type>ArtOrDet</type><correction>the
percentages</correction><comments/></mistake><mistake id="m1248958143519"
start="/0.139"
end="/0.161"><type>Um</type><correction/><comments/></mistake><mistake
id="m1248958228807" start="/3.74" end="/3.88"><type>Wcip</type><correction>are
similar with</correction><comments/></mistake><mistake id="m1248958276910"
start="/3.89" end="/3.103"><type>Mec</type><correction>teachers\'
view</correction><comments/></mistake><mistake id="m1248958311951" start="/3.223"
end="/3.227"><type>ArtOrDet</type><correction>the
same</correction><comments/></mistake><mistake id="m1248958330206" start="/3.242"
end="/3.245"><type>Um</type><correction/><comments/></mistake><mistake
id="m1248958374081" start="/3.294"
end="/3.297"><type>Vt</type><correction>are</correction><comments></comments></mistake><mistake
id="m1248958392755" start="/3.294" end="/3.297"><type>SVA</type><correction>are<%
XML;
*/

error_reporting(0);

$xmlstr = <<< XML
<?xml version='1.0' standalone='yes'?>
<movies>
 <movie>
  <titles>PHP: Behind the Parser</titles>
 </movied>
</movies>
XML;

/*** create a SimpleXML object ***/
    if( ! $xml = simplexml_load_string($xmlstr) )
	{
		header("HTTP/1.1 404 Not Found", true, 404);
	}
	else {
//		db_query($sql_insert, array($record->nid, $record->vid, $user->uid, $_REQUEST['xml']));
//		echo $_REQUEST['xml'];
		echo "everything ok";
	}

//$doc = simplexml_load_string($xmlstr);
//$xml = explode("\n", $xmlstr);
//
//if (!$doc) {
//    $errors = libxml_get_errors();
//
//    foreach ($errors as $error) {
//        echo display_xml_error($error, $xml);
//    }
//
//    libxml_clear_errors();
//}
//
//
//function display_xml_error($error, $xml)
//{
//    $return  = $xml[$error->line - 1] . "\n";
//    $return .= str_repeat('-', $error->column) . "^\n";
//
//    switch ($error->level) {
//        case LIBXML_ERR_WARNING:
//            $return .= "Warning $error->code: ";
//            break;
//         case LIBXML_ERR_ERROR:
//            $return .= "Error $error->code: ";
//            break;
//        case LIBXML_ERR_FATAL:
//            $return .= "Fatal Error $error->code: ";
//            break;
//    }
//
//    $return .= trim($error->message) .
//               "\n  Line: $error->line" .
//               "\n  Column: $error->column";
//
//    if ($error->file) {
//        $return .= "\n  File: $error->file";
//    }
//
//    return "$return\n\n--------------------------------------------\n\n";
//}
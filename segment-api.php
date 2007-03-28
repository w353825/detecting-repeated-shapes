<?php
/** 
 * Accepts a form submission for segment
 * segments and returns a page with the segmented image and the original image
 * TODO: better filename handing
 * SECURITY IMPLICATIONS:
 * - filename handling is weak.
 * - segment probably has attack vectors
 * CONCLUSION: run this in some sort of isolated area. 
 */

/* convert a file to a url */
if(!isset($_POST['url']) || $_POST['url'] == '') {
	$file = $_FILES['data'];
	if($file['size'] > 3000000) {
		die('ERROR: file is too big: size=' . $file['size'] . ' name=' . $file['name']);
	}

	$uploaddir = '/home/cmd144/apps.tobydietrich.com/tmp/';
	$filename = safe_unix_filename_encode(basename($file['name']));
	$uploadfile = $uploaddir . $filename;
	move_uploaded_file($file['tmp_name'], $uploadfile) or die ('ERROR: file not moved: ' . basename($file['name']));
	$_POST['url'] = 'http://apps.tobydietrich.com/tmp/' . $filename;
}

if(isset($_POST['url']) && $_POST['url'] != '')  {
	$url = (isset($_POST['url'])?urldecode($_POST['url']):'');
	$sigma = floatval($_POST['sigma']);
	$k = floatval($_POST['k']);
	$min = floatval($_POST['min']);
	$output_type = intval($_POST['output_type']);

	do_segment($url, $sigma, $k, $min, $output_type);
}


function get_ext ($url) {
	$arr = explode('.', $url);
	if(count($arr) <= 1) {
		return '';
	} else {
	    $ext = safe_unix_filename_encode($arr[count($arr)-1]);
		// FIXME: hack to remove obviously wrong extensions.
		if(strlen($ext) > 4) {
		   return '';
		} else {
		   return '.' . $ext;
		}
	}
}
function do_segment($url, $sigma, $k, $min, $output_type) {
   // get the image
   $image = fetch_image($url);

   $id = uniqid();
   $ext = get_ext($url);
   $disk_filename = get_orig_filename($id, $ext);
   
   // write to filesystem
   if(!is_file($disk_filename)) {
      $handle = fopen($disk_filename, 'x');
      $handle or die ("ERROR: Cannot open file: $disk_filename");
      fwrite($handle, $image) or die ("ERROR: Cannot write to file: $disk_filename");
      fclose($handle);
   } else {
      die("ERROR: File exists: $disk_filename");
   }

   // the file is now on disk

   convert("$disk_filename " . get_ppm_in_filename($id));
   segment("$sigma $k $min 0 0 0 0 0 0 0 0 0 $output_type " . get_ppm_in_filename($id) . ' ' . get_ppm_out_filename($id));
   convert(get_ppm_out_filename($id) . ' ' .get_segment_filename($id, $ext));
   
   ?>
   <ul>
   <li><img src="<?= get_orig_url($id, $ext) ?>" alt="original" /></li>
   <li><img src="<?= get_segment_url($id, $ext); ?>" alt="segment" /></li>
   </ul>
   <?php
}
function convert ( $command ) {
	exec("/usr/bin/convert $command");
} 

function segment ($command) {
	exec("/home/cmd144/packages/bin/segment $command");
}
/****
 * returns the image
 *
 * @param unknown_type $url
 * @return unknown
 */
function fetch_image($url) {
   $ch = curl_init($url);
   curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
   curl_setopt($ch, CURLOPT_TIMEOUT, 4);
   $image = curl_exec($ch);
   curl_close($ch);
   return $image;
}

function get_orig_filename( $id, $ext ) {
   return "/home/cmd144/apps.tobydietrich.com/images/$id-orig$ext";
}

function get_negate_filename ( $id,$ext ) {
	if($ext == '') {
		$ext = ".jpg";
	}
	return "/home/cmd144/apps.tobydietrich.com/images/$id-negate$ext";
}

function get_segment_filename ( $id, $ext ) {
	if($ext == '') {
		$ext = ".jpg";
	}
	return "/home/cmd144/apps.tobydietrich.com/images/$id-segment$ext";
}

function get_ppm_in_filename( $id) {
	return "/home/cmd144/apps.tobydietrich.com/images/$id-ppm-in.ppm";
}

function get_ppm_out_filename ( $id ) {
	return "/home/cmd144/apps.tobydietrich.com/images/$id-ppm-out.ppm";
}


function get_orig_url( $id,$ext ) {
   return "http://apps.tobydietrich.com/images/$id-orig$ext";
}

function get_negate_url ( $id,$ext ) {
	if($ext == '') {
		$ext = ".jpg";
	}
	return "http://apps.tobydietrich.com/images/$id-negate$ext";
}

function get_segment_url ( $id,$ext ) {
	if($ext == '') {
		$ext = ".jpg";
	}
	return "http://apps.tobydietrich.com/images/$id-segment$ext";
}

// FIXME
function safe_unix_filename_encode( $filename ) {
	$filename = str_replace(' ', '_', $filename);
	$filename = str_replace('/', '_forwardslash_', $filename);
	$filename = str_replace('\\', '_backwardslash_', $filename);
    $filename = str_replace(':', '_colon_', $filename);
    $filename = str_replace(';', '_semicolon_', $filename);
	$filename = str_replace('&', '_amp_', $filename);
    $filename = str_replace('?', '_question_', $filename);
	$filename = str_replace('=', '_equal_', $filename);
	$filename = str_replace('(', '_openparen_', $filename);
	$filename = str_replace(')', '_closeparen_', $filename);
	if(strlen($filename) > 50) {
	   return substr($filename, strlen($filename)-50);
	} else {
	   return $filename;
	}
}

function die( $str ) {

}
?>
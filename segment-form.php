<form action="http://apps.tobydietrich.com/segment-api.php" method="post" enctype="multipart/form-data">
<p>URL<br />
<input name="url" type="text" size="50" />
<p>OR</p>
<input type="hidden" name="MAX_FILE_SIZE" value="2000000" />

File<br />
<input type="file" name="data" size="50" />
<p>sigma<br />
<input name="sigma" value="0.5" type="text" size="8" />
<p>k<br />
<input name="k" value="500" type="text" size="8" />
<p>min<br />
<input name="min" value="20" type="text" size="8" />
<p>output type<br />
<select name="output_type">
<option value="0" selected>false color</option><br />

<option value="1">average</option><br />
<option value="2">greyscale</option><br />
</select>
<p></p>
<input value="submit" type="submit" />
</form>
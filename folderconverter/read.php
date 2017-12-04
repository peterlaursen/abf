<?php
$handle = fopen("metadata.txt", "r");
$chaptersread = 0;

while ($line = fgets($handle)) {

if ($line == "[CHAPTER]\n") {
$timebase = fgets($handle);
$start = fgets($handle);
$end = fgets($handle);
$startduration;
sscanf($start, "START=%d\n", $startduration);
$endduration;
sscanf($end, "END=%d\n", $endduration);
$startduration = ($startduration>0)?$startduration/1000:$startduration;
$endduration/=1000;
printf("Start: %.3f, End: %.3f\n", $startduration, $endduration);
system(sprintf("ffmpeg -hide_banner -y -ss %.3f -i Original.aac -ac 1 -t %.3f %04d.mp3", $startduration, $endduration - $startduration, $chaptersread));
printf("\n");
++$chaptersread;

}
}

fclose($handle);


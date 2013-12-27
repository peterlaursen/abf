This is the original branch of ABF 1.0.
The format is beginning to show its age design-wise.
While the audio is fine and the converter does its job, it is slow and requires too much disk space for temporary files.

With the release of FreeBSD 10, a new function (fmemopen) has made it possible for us to decode the audio, do resampling and a lot more in memory. This is excellent news for us, since it makes our job a little easier.

Character set conversion must also be done. ABF has never supported internationalization, so characters (especially Danish ones) would often be shown wrong. I shall rectify this in ABF 2.0.
I therefore leave this code around in a abf-1.0 branch so that I, at some time, can look back at this fifth semester school project with pride.


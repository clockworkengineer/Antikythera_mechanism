#include "HOST.hpp"
/*
 * File:   CMIME.cpp
 * 
 * Author: Robert Tizzard
 * 
 * Created on October 24, 2016, 2:33 PM
 *
 * Copyright 2016.
 *
 */

//
// Class: CMIME
// 
// Description: Class to provide file extension to MIME type mapping, amongst
// other MIME processing functionality.
//
// Dependencies:   C11++     - Language standard features used.
//                 CMailSMTP - Base64 decoding.
//

// =================
// CLASS DEFINITIONS
// =================

#include "CMIME.hpp"

// ====================
// CLASS IMPLEMENTATION
// ====================

//
// C++ STL
//

#include <cstring>

// =========
// NAMESPACE
// =========

namespace Antik {
    namespace File {

    // ===========================
    // PRIVATE TYPES AND CONSTANTS
    // ===========================

    //
    // MIME encoded word constants
    //

    const char *CMIME::kEncodedWordPrefix { "=?" };
    const char *CMIME::kEncodedWordPostfix { "?=" };
    const char *CMIME::kEncodedWordSeparator { "?" };
    const char *CMIME::kEncodedWordASCII { "ASCII" };
    const char CMIME::kEncodedWordTypeBase64 { 'B' };
    const char CMIME::kEncodedWordTypeQuoted { 'Q' };
    const char CMIME::kEncodedWordTypeNone { ' ' };
    const char CMIME::kQuotedPrintPrefix { '=' };


    // ==========================
    // PUBLIC TYPES AND CONSTANTS
    // ==========================

    // ========================
    // PRIVATE STATIC VARIABLES
    // ========================

    // File extension to MIME type mapping table

    std::unordered_map<std::string, std::string> CMIME::m_extToMimeType
    {

        { "ez", "application/andrew-inset"},
        { "anx", "application/annodex"},
        { "atom", "application/atom+xml"},
        { "atomcat", "application/atomcat+xml"},
        { "atomsrv", "application/atomserv+xml"},
        { "lin", "application/bbolin"},
        { "cu", "application/cu-seeme"},
        { "davmount", "application/davmount+xml"},
        { "dcm", "application/dicom"},
        { "tsp", "application/dsptype"},
        { "es", "application/ecmascript"},
        { "otf", "application/font-sfnt"},
        { "ttf", "application/font-sfnt"},
        { "pfr", "application/font-tdpfr"},
        { "woff", "application/font-woff"},
        { "spl", "application/futuresplash"},
        { "gz", "application/gzip"},
        { "hta", "application/hta"},
        { "jar", "application/java-archive"},
        { "ser", "application/java-serialized-object"},
        { "class", "application/java-vm"},
        { "js", "application/javascript"},
        { "json", "application/json"},
        { "m3g", "application/m3g"},
        { "hqx", "application/mac-binhex40"},
        { "cpt", "application/mac-compactpro"},
        { "nb", "application/mathematica"},
        { "nbp", "application/mathematica"},
        { "mbox", "application/mbox"},
        { "mdb", "application/msaccess"},
        { "doc", "application/msword"},
        { "dot", "application/msword"},
        { "mxf", "application/mxf"},
        { "bin", "application/octet-stream"},
        { "oda", "application/oda"},
        { "opf", "application/oebps-package+xml"},
        { "ogx", "application/ogg"},
        { "one", "application/onenote"},
        { "onetoc2", "application/onenote"},
        { "onetmp", "application/onenote"},
        { "onepkg", "application/onenote"},
        { "pdf", "application/pdf"},
        { "pgp", "application/pgp-encrypted"},
        { "key", "application/pgp-keys"},
        { "sig", "application/pgp-signature"},
        { "prf", "application/pics-rules"},
        { "ps", "application/postscript"},
        { "ai", "application/postscript"},
        { "eps", "application/postscript"},
        { "epsi", "application/postscript"},
        { "epsf", "application/postscript"},
        { "eps2", "application/postscript"},
        { "eps3", "application/postscript"},
        { "rar", "application/rar"},
        { "rdf", "application/rdf+xml"},
        { "rtf", "application/rtf"},
        { "stl", "application/sla"},
        { "smi", "application/smil+xml"},
        { "smil", "application/smil+xml"},
        { "xhtml", "application/xhtml+xml"},
        { "xht", "application/xhtml+xml"},
        { "xml", "application/xml"},
        { "xsd", "application/xml"},
        { "xsl", "application/xslt+xml"},
        { "xslt", "application/xslt+xml"},
        { "xspf", "application/xspf+xml"},
        { "zip", "application/zip"},
        { "apk", "application/vnd.android.package-archive"},
        { "cdy", "application/vnd.cinderella"},
        { "deb", "application/vnd.debian.binary-package"},
        { "ddeb", "application/vnd.debian.binary-package"},
        { "udeb", "application/vnd.debian.binary-package"},
        { "sfd", "application/vnd.font-fontforge-sfd"},
        { "kml", "application/vnd.google-earth.kml+xml"},
        { "kmz", "application/vnd.google-earth.kmz"},
        { "xul", "application/vnd.mozilla.xul+xml"},
        { "xls", "application/vnd.ms-excel"},
        { "xlb", "application/vnd.ms-excel"},
        { "xlt", "application/vnd.ms-excel"},
        { "xlam", "application/vnd.ms-excel.addin.macroEnabled.12"},
        { "xlsb", "application/vnd.ms-excel.sheet.binary.macroEnabled.12"},
        { "xlsm", "application/vnd.ms-excel.sheet.macroEnabled.12"},
        { "xltm", "application/vnd.ms-excel.template.macroEnabled.12"},
        { "eot", "application/vnd.ms-fontobject"},
        { "thmx", "application/vnd.ms-officetheme"},
        { "cat", "application/vnd.ms-pki.seccat"},
        { "ppt", "application/vnd.ms-powerpoint"},
        { "pps", "application/vnd.ms-powerpoint"},
        { "ppam", "application/vnd.ms-powerpoint.addin.macroEnabled.12"},
        { "pptm", "application/vnd.ms-powerpoint.presentation.macroEnabled.12"},
        { "sldm", "application/vnd.ms-powerpoint.slide.macroEnabled.12"},
        { "ppsm", "application/vnd.ms-powerpoint.slideshow.macroEnabled.12"},
        { "potm", "application/vnd.ms-powerpoint.template.macroEnabled.12"},
        { "docm", "application/vnd.ms-word.document.macroEnabled.12"},
        { "dotm", "application/vnd.ms-word.template.macroEnabled.12"},
        { "odc", "application/vnd.oasis.opendocument.chart"},
        { "odb", "application/vnd.oasis.opendocument.database"},
        { "odf", "application/vnd.oasis.opendocument.formula"},
        { "odg", "application/vnd.oasis.opendocument.graphics"},
        { "otg", "application/vnd.oasis.opendocument.graphics-template"},
        { "odi", "application/vnd.oasis.opendocument.image"},
        { "odp", "application/vnd.oasis.opendocument.presentation"},
        { "otp", "application/vnd.oasis.opendocument.presentation-template"},
        { "ods", "application/vnd.oasis.opendocument.spreadsheet"},
        { "ots", "application/vnd.oasis.opendocument.spreadsheet-template"},
        { "odt", "application/vnd.oasis.opendocument.text"},
        { "odm", "application/vnd.oasis.opendocument.text-master"},
        { "ott", "application/vnd.oasis.opendocument.text-template"},
        { "oth", "application/vnd.oasis.opendocument.text-web"},
        { "pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
        { "sldx", "application/vnd.openxmlformats-officedocument.presentationml.slide"},
        { "ppsx", "application/vnd.openxmlformats-officedocument.presentationml.slideshow"},
        { "potx", "application/vnd.openxmlformats-officedocument.presentationml.template"},
        { "xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
        { "xltx", "application/vnd.openxmlformats-officedocument.spreadsheetml.template"},
        { "docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
        { "dotx", "application/vnd.openxmlformats-officedocument.wordprocessingml.template"},
        { "cod", "application/vnd.rim.cod"},
        { "mmf", "application/vnd.smaf"},
        { "sdc", "application/vnd.stardivision.calc"},
        { "sds", "application/vnd.stardivision.chart"},
        { "sda", "application/vnd.stardivision.draw"},
        { "sdd", "application/vnd.stardivision.impress"},
        { "sdf", "application/vnd.stardivision.math"},
        { "sdw", "application/vnd.stardivision.writer"},
        { "sgl", "application/vnd.stardivision.writer-global"},
        { "sxc", "application/vnd.sun.xml.calc"},
        { "stc", "application/vnd.sun.xml.calc.template"},
        { "sxd", "application/vnd.sun.xml.draw"},
        { "std", "application/vnd.sun.xml.draw.template"},
        { "sxi", "application/vnd.sun.xml.impress"},
        { "sti", "application/vnd.sun.xml.impress.template"},
        { "sxm", "application/vnd.sun.xml.math"},
        { "sxw", "application/vnd.sun.xml.writer"},
        { "sxg", "application/vnd.sun.xml.writer.global"},
        { "stw", "application/vnd.sun.xml.writer.template"},
        { "sis", "application/vnd.symbian.install"},
        { "cap", "application/vnd.tcpdump.pcap"},
        { "pcap", "application/vnd.tcpdump.pcap"},
        { "vsd", "application/vnd.visio"},
        { "vst", "application/vnd.visio"},
        { "vsw", "application/vnd.visio"},
        { "vss", "application/vnd.visio"},
        { "wbxml", "application/vnd.wap.wbxml"},
        { "wmlc", "application/vnd.wap.wmlc"},
        { "wmlsc", "application/vnd.wap.wmlscriptc"},
        { "wpd", "application/vnd.wordperfect"},
        { "wp5", "application/vnd.wordperfect5.1"},
        { "wk", "application/x-123"},
        { "7z", "application/x-7z-compressed"},
        { "abw", "application/x-abiword"},
        { "dmg", "application/x-apple-diskimage"},
        { "bcpio", "application/x-bcpio"},
        { "torrent", "application/x-bittorrent"},
        { "cab", "application/x-cab"},
        { "cbr", "application/x-cbr"},
        { "cbz", "application/x-cbz"},
        { "cdf", "application/x-cdf"},
        { "cda", "application/x-cdf"},
        { "vcd", "application/x-cdlink"},
        { "pgn", "application/x-chess-pgn"},
        { "mph", "application/x-comsol"},
        { "cpio", "application/x-cpio"},
        { "csh", "application/x-csh"},
        { "deb", "application/x-debian-package"},
        { "udeb", "application/x-debian-package"},
        { "dcr", "application/x-director"},
        { "dir", "application/x-director"},
        { "dxr", "application/x-director"},
        { "dms", "application/x-dms"},
        { "wad", "application/x-doom"},
        { "dvi", "application/x-dvi"},
        { "pfa", "application/x-font"},
        { "pfb", "application/x-font"},
        { "gsf", "application/x-font"},
        { "pcf", "application/x-font-pcf"},
        { "pcf.Z", "application/x-font-pcf"},
        { "mm", "application/x-freemind"},
        { "spl", "application/x-futuresplash"},
        { "gan", "application/x-ganttproject"},
        { "gnumeric", "application/x-gnumeric"},
        { "sgf", "application/x-go-sgf"},
        { "gcf", "application/x-graphing-calculator"},
        { "gtar", "application/x-gtar"},
        { "tgz", "application/x-gtar-compressed"},
        { "taz", "application/x-gtar-compressed"},
        { "hdf", "application/x-hdf"},
        { "hwp", "application/x-hwp"},
        { "ica", "application/x-ica"},
        { "info", "application/x-info"},
        { "ins", "application/x-internet-signup"},
        { "isp", "application/x-internet-signup"},
        { "iii", "application/x-iphone"},
        { "iso", "application/x-iso9660-image"},
        { "jam", "application/x-jam"},
        { "jnlp", "application/x-java-jnlp-file"},
        { "jmz", "application/x-jmol"},
        { "chrt", "application/x-kchart"},
        { "kil", "application/x-killustrator"},
        { "skp", "application/x-koan"},
        { "skd", "application/x-koan"},
        { "skt", "application/x-koan"},
        { "skm", "application/x-koan"},
        { "kpr", "application/x-kpresenter"},
        { "kpt", "application/x-kpresenter"},
        { "ksp", "application/x-kspread"},
        { "kwd", "application/x-kword"},
        { "kwt", "application/x-kword"},
        { "latex", "application/x-latex"},
        { "lha", "application/x-lha"},
        { "lyx", "application/x-lyx"},
        { "lzh", "application/x-lzh"},
        { "lzx", "application/x-lzx"},
        { "frm", "application/x-maker"},
        { "maker", "application/x-maker"},
        { "frame", "application/x-maker"},
        { "fm", "application/x-maker"},
        { "fb", "application/x-maker"},
        { "book", "application/x-maker"},
        { "fbdoc", "application/x-maker"},
        { "mif", "application/x-mif"},
        { "m3u8", "application/x-mpegURL"},
        { "wmd", "application/x-ms-wmd"},
        { "wmz", "application/x-ms-wmz"},
        { "com", "application/x-msdos-program"},
        { "exe", "application/x-msdos-program"},
        { "bat", "application/x-msdos-program"},
        { "dll", "application/x-msdos-program"},
        { "msi", "application/x-msi"},
        { "nc", "application/x-netcdf"},
        { "pac", "application/x-ns-proxy-autoconfig"},
        { "nwc", "application/x-nwc"},
        { "o", "application/x-object"},
        { "oza", "application/x-oz-application"},
        { "p7r", "application/x-pkcs7-certreqresp"},
        { "crl", "application/x-pkcs7-crl"},
        { "pyc", "application/x-python-code"},
        { "pyo", "application/x-python-code"},
        { "qgs", "application/x-qgis"},
        { "shp", "application/x-qgis"},
        { "shx", "application/x-qgis"},
        { "qtl", "application/x-quicktimeplayer"},
        { "rdp", "application/x-rdp"},
        { "rpm", "application/x-redhat-package-manager"},
        { "rss", "application/x-rss+xml"},
        { "rb", "application/x-ruby"},
        { "sci", "application/x-scilab"},
        { "sce", "application/x-scilab"},
        { "xcos", "application/x-scilab-xcos"},
        { "sh", "application/x-sh"},
        { "shar", "application/x-shar"},
        { "swf", "application/x-shockwave-flash"},
        { "swfl", "application/x-shockwave-flash"},
        { "scr", "application/x-silverlight"},
        { "sql", "application/x-sql"},
        { "sit", "application/x-stuffit"},
        { "sitx", "application/x-stuffit"},
        { "sv4cpio", "application/x-sv4cpio"},
        { "sv4crc", "application/x-sv4crc"},
        { "tar", "application/x-tar"},
        { "tcl", "application/x-tcl"},
        { "gf", "application/x-tex-gf"},
        { "pk", "application/x-tex-pk"},
        { "texinfo", "application/x-texinfo"},
        { "texi", "application/x-texinfo"},
        { "~", "application/x-trash"},
        { "%", "application/x-trash"},
        { "bak", "application/x-trash"},
        { "old", "application/x-trash"},
        { "sik", "application/x-trash"},
        { "t", "application/x-troff"},
        { "tr", "application/x-troff"},
        { "roff", "application/x-troff"},
        { "man", "application/x-troff-man"},
        { "me", "application/x-troff-me"},
        { "ms", "application/x-troff-ms"},
        { "ustar", "application/x-ustar"},
        { "src", "application/x-wais-source"},
        { "wz", "application/x-wingz"},
        { "crt", "application/x-x509-ca-cert"},
        { "xcf", "application/x-xcf"},
        { "fig", "application/x-xfig"},
        { "xpi", "application/x-xpinstall"},
        { "xz", "application/x-xz"},
        { "amr", "audio/amr"},
        { "awb", "audio/amr-wb"},
        { "axa", "audio/annodex"},
        { "au", "audio/basic"},
        { "snd", "audio/basic"},
        { "csd", "audio/csound"},
        { "orc", "audio/csound"},
        { "sco", "audio/csound"},
        { "flac", "audio/flac"},
        { "mid", "audio/midi"},
        { "midi", "audio/midi"},
        { "kar", "audio/midi"},
        { "mpga", "audio/mpeg"},
        { "mpega", "audio/mpeg"},
        { "mp2", "audio/mpeg"},
        { "mp3", "audio/mpeg"},
        { "m4a", "audio/mpeg"},
        { "m3u", "audio/mpegurl"},
        { "oga", "audio/ogg"},
        { "ogg", "audio/ogg"},
        { "opus", "audio/ogg"},
        { "spx", "audio/ogg"},
        { "sid", "audio/prs.sid"},
        { "aif", "audio/x-aiff"},
        { "aiff", "audio/x-aiff"},
        { "aifc", "audio/x-aiff"},
        { "gsm", "audio/x-gsm"},
        { "m3u", "audio/x-mpegurl"},
        { "wma", "audio/x-ms-wma"},
        { "wax", "audio/x-ms-wax"},
        { "ra", "audio/x-pn-realaudio"},
        { "rm", "audio/x-pn-realaudio"},
        { "ram", "audio/x-pn-realaudio"},
        { "ra", "audio/x-realaudio"},
        { "pls", "audio/x-scpls"},
        { "sd2", "audio/x-sd2"},
        { "wav", "audio/x-wav"},
        { "alc", "chemical/x-alchemy"},
        { "cac", "chemical/x-cache"},
        { "cache", "chemical/x-cache"},
        { "csf", "chemical/x-cache-csf"},
        { "cbin", "chemical/x-cactvs-binary"},
        { "cascii", "chemical/x-cactvs-binary"},
        { "ctab", "chemical/x-cactvs-binary"},
        { "cdx", "chemical/x-cdx"},
        { "cer", "chemical/x-cerius"},
        { "c3d", "chemical/x-chem3d"},
        { "chm", "chemical/x-chemdraw"},
        { "cif", "chemical/x-cif"},
        { "cmdf", "chemical/x-cmdf"},
        { "cml", "chemical/x-cml"},
        { "cpa", "chemical/x-compass"},
        { "bsd", "chemical/x-crossfire"},
        { "csml", "chemical/x-csml"},
        { "csm", "chemical/x-csml"},
        { "ctx", "chemical/x-ctx"},
        { "cxf", "chemical/x-cxf"},
        { "cef", "chemical/x-cxf"},
        { "emb", "chemical/x-embl-dl-nucleotide"},
        { "embl", "chemical/x-embl-dl-nucleotide"},
        { "spc", "chemical/x-galactic-spc"},
        { "inp", "chemical/x-gamess-input"},
        { "gam", "chemical/x-gamess-input"},
        { "gamin", "chemical/x-gamess-input"},
        { "fch", "chemical/x-gaussian-checkpoint"},
        { "fchk", "chemical/x-gaussian-checkpoint"},
        { "cub", "chemical/x-gaussian-cube"},
        { "gau", "chemical/x-gaussian-input"},
        { "gjc", "chemical/x-gaussian-input"},
        { "gjf", "chemical/x-gaussian-input"},
        { "gal", "chemical/x-gaussian-log"},
        { "gcg", "chemical/x-gcg8-sequence"},
        { "gen", "chemical/x-genbank"},
        { "hin", "chemical/x-hin"},
        { "istr", "chemical/x-isostar"},
        { "ist", "chemical/x-isostar"},
        { "jdx", "chemical/x-jcamp-dx"},
        { "dx", "chemical/x-jcamp-dx"},
        { "kin", "chemical/x-kinemage"},
        { "mcm", "chemical/x-macmolecule"},
        { "mmd", "chemical/x-macromodel-input"},
        { "mmod", "chemical/x-macromodel-input"},
        { "mol", "chemical/x-mdl-molfile"},
        { "rd", "chemical/x-mdl-rdfile"},
        { "rxn", "chemical/x-mdl-rxnfile"},
        { "sd", "chemical/x-mdl-sdfile"},
        { "sdf", "chemical/x-mdl-sdfile"},
        { "tgf", "chemical/x-mdl-tgf"},
        { "mcif", "chemical/x-mmcif"},
        { "mol2", "chemical/x-mol2"},
        { "b", "chemical/x-molconn-Z"},
        { "gpt", "chemical/x-mopac-graph"},
        { "mop", "chemical/x-mopac-input"},
        { "mopcrt", "chemical/x-mopac-input"},
        { "mpc", "chemical/x-mopac-input"},
        { "zmt", "chemical/x-mopac-input"},
        { "moo", "chemical/x-mopac-out"},
        { "mvb", "chemical/x-mopac-vib"},
        { "asn", "chemical/x-ncbi-asn1"},
        { "prt", "chemical/x-ncbi-asn1-ascii"},
        { "ent", "chemical/x-ncbi-asn1-ascii"},
        { "val", "chemical/x-ncbi-asn1-binary"},
        { "aso", "chemical/x-ncbi-asn1-binary"},
        { "asn", "chemical/x-ncbi-asn1-spec"},
        { "pdb", "chemical/x-pdb"},
        { "ent", "chemical/x-pdb"},
        { "ros", "chemical/x-rosdal"},
        { "sw", "chemical/x-swissprot"},
        { "vms", "chemical/x-vamas-iso14976"},
        { "vmd", "chemical/x-vmd"},
        { "xtel", "chemical/x-xtel"},
        { "xyz", "chemical/x-xyz"},
        { "gif", "image/gif"},
        { "ief", "image/ief"},
        { "jp2", "image/jp2"},
        { "jpg2", "image/jp2"},
        { "jpeg", "image/jpeg"},
        { "jpg", "image/jpeg"},
        { "jpe", "image/jpeg"},
        { "jpm", "image/jpm"},
        { "jpx", "image/jpx"},
        { "jpf", "image/jpx"},
        { "pcx", "image/pcx"},
        { "png", "image/png"},
        { "svg", "image/svg+xml"},
        { "svgz", "image/svg+xml"},
        { "tiff", "image/tiff"},
        { "tif", "image/tiff"},
        { "djvu", "image/vnd.djvu"},
        { "djv", "image/vnd.djvu"},
        { "ico", "image/vnd.microsoft.icon"},
        { "wbmp", "image/vnd.wap.wbmp"},
        { "cr2", "image/x-canon-cr2"},
        { "crw", "image/x-canon-crw"},
        { "ras", "image/x-cmu-raster"},
        { "cdr", "image/x-coreldraw"},
        { "pat", "image/x-coreldrawpattern"},
        { "cdt", "image/x-coreldrawtemplate"},
        { "cpt", "image/x-corelphotopaint"},
        { "erf", "image/x-epson-erf"},
        { "art", "image/x-jg"},
        { "jng", "image/x-jng"},
        { "bmp", "image/x-ms-bmp"},
        { "nef", "image/x-nikon-nef"},
        { "orf", "image/x-olympus-orf"},
        { "psd", "image/x-photoshop"},
        { "pnm", "image/x-portable-anymap"},
        { "pbm", "image/x-portable-bitmap"},
        { "pgm", "image/x-portable-graymap"},
        { "ppm", "image/x-portable-pixmap"},
        { "rgb", "image/x-rgb"},
        { "xbm", "image/x-xbitmap"},
        { "xpm", "image/x-xpixmap"},
        { "xwd", "image/x-xwindowdump"},
        { "eml", "message/rfc822"},
        { "igs", "model/iges"},
        { "iges", "model/iges"},
        { "msh", "model/mesh"},
        { "mesh", "model/mesh"},
        { "silo", "model/mesh"},
        { "wrl", "model/vrml"},
        { "vrml", "model/vrml"},
        { "x3dv", "model/x3d+vrml"},
        { "x3d", "model/x3d+xml"},
        { "x3db", "model/x3d+binary"},
        { "appcache", "text/cache-manifest"},
        { "ics", "text/calendar"},
        { "icz", "text/calendar"},
        { "css", "text/css"},
        { "csv", "text/csv"},
        { "323", "text/h323"},
        { "html", "text/html"},
        { "htm", "text/html"},
        { "shtml", "text/html"},
        { "uls", "text/iuls"},
        { "mml", "text/mathml"},
        { "asc", "text/plain"},
        { "txt", "text/plain"},
        { "text", "text/plain"},
        { "pot", "text/plain"},
        { "brf", "text/plain"},
        { "srt", "text/plain"},
        { "rtx", "text/richtext"},
        { "sct", "text/scriptlet"},
        { "wsc", "text/scriptlet"},
        { "tm", "text/texmacs"},
        { "tsv", "text/tab-separated-values"},
        { "ttl", "text/turtle"},
        { "vcf", "text/vcard"},
        { "vcard", "text/vcard"},
        { "jad", "text/vnd.sun.j2me.app-descriptor"},
        { "wml", "text/vnd.wap.wml"},
        { "wmls", "text/vnd.wap.wmlscript"},
        { "bib", "text/x-bibtex"},
        { "boo", "text/x-boo"},
        { "h++", "text/x-c++hdr"},
        { "hpp", "text/x-c++hdr"},
        { "hxx", "text/x-c++hdr"},
        { "hh", "text/x-c++hdr"},
        { "c++", "text/x-c++src"},
        { "cpp", "text/x-c++src"},
        { "cxx", "text/x-c++src"},
        { "cc", "text/x-c++src"},
        { "h", "text/x-chdr"},
        { "htc", "text/x-component"},
        { "csh", "text/x-csh"},
        { "c", "text/x-csrc"},
        { "d", "text/x-dsrc"},
        { "diff", "text/x-diff"},
        { "patch", "text/x-diff"},
        { "hs", "text/x-haskell"},
        { "java", "text/x-java"},
        { "ly", "text/x-lilypond"},
        { "lhs", "text/x-literate-haskell"},
        { "moc", "text/x-moc"},
        { "p", "text/x-pascal"},
        { "pas", "text/x-pascal"},
        { "gcd", "text/x-pcs-gcd"},
        { "pl", "text/x-perl"},
        { "pm", "text/x-perl"},
        { "py", "text/x-python"},
        { "scala", "text/x-scala"},
        { "etx", "text/x-setext"},
        { "sfv", "text/x-sfv"},
        { "sh", "text/x-sh"},
        { "tcl", "text/x-tcl"},
        { "tk", "text/x-tcl"},
        { "tex", "text/x-tex"},
        { "ltx", "text/x-tex"},
        { "sty", "text/x-tex"},
        { "cls", "text/x-tex"},
        { "vcs", "text/x-vcalendar"},
        { "3gp", "video/3gpp"},
        { "axv", "video/annodex"},
        { "dl", "video/dl"},
        { "dif", "video/dv"},
        { "dv", "video/dv"},
        { "fli", "video/fli"},
        { "gl", "video/gl"},
        { "mpeg", "video/mpeg"},
        { "mpg", "video/mpeg"},
        { "mpe", "video/mpeg"},
        { "ts", "video/MP2T"},
        { "mp4", "video/mp4"},
        { "qt", "video/quicktime"},
        { "mov", "video/quicktime"},
        { "ogv", "video/ogg"},
        { "webm", "video/webm"},
        { "mxu", "video/vnd.mpegurl"},
        { "flv", "video/x-flv"},
        { "lsf", "video/x-la-asf"},
        { "lsx", "video/x-la-asf"},
        { "mng", "video/x-mng"},
        { "asf", "video/x-ms-asf"},
        { "asx", "video/x-ms-asf"},
        { "wm", "video/x-ms-wm"},
        { "wmv", "video/x-ms-wmv"},
        { "wmx", "video/x-ms-wmx"},
        { "wvx", "video/x-ms-wvx"},
        { "avi", "video/x-msvideo"},
        { "movie", "video/x-sgi-movie"},
        { "mpv", "video/x-matroska"},
        { "mkv", "video/x-matroska"},
        { "ice", "x-conference/x-cooltalk"},
        { "sisx", "x-epoc/x-sisx-app"},
        { "vrm", "x-world/x-vrml"},
        { "vrml", "x-world/x-vrml"},
        { "wrl", "x-world/x-vrml"}};

    // =======================
    // PUBLIC STATIC VARIABLES
    // =======================

    // ===============
    // PRIVATE METHODS
    // ===============

    // ==============
    // PUBLIC METHODS
    // ==============

    //
    // Return MIME type for files extension using internal mapping table.
    // If none found then return "application/unknown".
    //

    std::string CMIME::getFileMIMEType(const std::string& fileName) {

        std::string baseFileName { fileName.substr(fileName.find_last_of("/\\") + 1) };
        std::size_t fullStop { baseFileName.find_last_of('.') };

        if (fullStop != std::string::npos) {

            auto foundMapping = m_extToMimeType.find(baseFileName.substr(fullStop + 1));
            if (foundMapping != m_extToMimeType.end()) {
                return (foundMapping->second);
            }

        }

        return ("application/unknown");

    }

    //
    // Parse MIME string into normal ASCII and MIME word encoded pieces.
    //

    std::vector<CMIME::ParsedMIMEing> CMIME::parseMIMEing(const std::string& mime) {

        std::istringstream subjecteam(mime);
        ParsedMIMEing parsedEntry;
        std::vector<ParsedMIMEing> parseding;

        for (std::string line; std::getline(subjecteam, line, '\n');) {

            line.pop_back();
            if (line.find_first_not_of(' ') != std::string::npos) {
                line = line.substr(line.find_first_not_of(' '));
            }

            while (!line.empty()) {

                parsedEntry.type = kEncodedWordTypeNone;
                parsedEntry.encoding = kEncodedWordASCII;

                if (line.find(kEncodedWordPrefix) == 0) {

                    line = line.substr(std::strlen(kEncodedWordPrefix));
                    parsedEntry.encoding = line.substr(0, line.find(kEncodedWordSeparator));
                    line = line.substr(line.find(kEncodedWordSeparator) + 1);

                    parsedEntry.type = line[0];
                    line = line.substr(std::strlen(kEncodedWordPrefix));
                    parsedEntry.contents = line.substr(0, line.find(kEncodedWordPostfix));
                    line = line.substr(line.find(kEncodedWordPostfix) + 2);

                } else {

                    if (line.find(kEncodedWordPostfix) == std::string::npos) {
                        parsedEntry.contents = line;
                        line = "";
                    } else {
                        parsedEntry.contents = line.substr(0, line.find(kEncodedWordPrefix));
                        line = line.substr(line.find(kEncodedWordPrefix));
                    }

                }

                parseding.push_back(parsedEntry);

            }
        }

        return (parseding);

    }

    //
    // Parse MIME string passed in and convert it to ASCII as best can.
    //

    std::string CMIME::convertMIMEStringToASCII(const std::string& mime) {

        std::vector<ParsedMIMEing> parseding { parseMIMEing(mime) };
        std::string convertedMIME;

        for (auto& parsedEntry : parseding) {
            if (parsedEntry.type == kEncodedWordTypeNone) {
                convertedMIME += parsedEntry.contents;
            } else if (parsedEntry.type == kEncodedWordTypeQuoted) {
                int cnt01 = 0;
                while (cnt01 < parsedEntry.contents.length()) {
                    if (parsedEntry.contents[cnt01] != kQuotedPrintPrefix) {
                        convertedMIME.append(1, parsedEntry.contents[cnt01++]);
                    } else {
                        unsigned char byte1, byte2;
                        cnt01++;
                        byte2 = parsedEntry.contents[cnt01++];
                        byte1 = ((byte2 <= '9') ? (byte2 - '0') : ((byte2 - 'A') + 10)) << 4;
                        byte2 = parsedEntry.contents[cnt01++];
                        byte1 |= ((byte2 <= '9') ? (byte2 - '0') : ((byte2 - 'A') + 10));
                        convertedMIME.append(1, byte1);
                    }
                }

            } else if (parsedEntry.type == kEncodedWordTypeBase64) {
                std::string decoded;
                Antik::SMTP::CSMTP::decodeFromBase64(parsedEntry.contents, decoded, parsedEntry.contents.length());
                convertedMIME += decoded;
            }

        }

        return (convertedMIME);

    }

    } // namespace File
} // namespace Antik

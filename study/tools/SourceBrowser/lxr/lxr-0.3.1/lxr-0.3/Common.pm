# $Id: Common.pm,v 1.6 1998/05/14 11:59:22 argggh Exp $

package LXR::Common;

use DB_File;

require Exporter;
@ISA = qw(Exporter);
@EXPORT = qw(&warning &fatal &abortall &fflush &urlargs 
	     &fileref &idref &htmlquote &freetextmarkup &markupfile
	     &init &makeheader &makefooter &expandtemplate);


$wwwdebug = 1;

$SIG{__WARN__} = 'warning';
$SIG{__DIE__}  = 'fatal';


@cterm = ('atom',	'\\\\.',	'',
	  'comment',	'/\*',		'\*/',
	  'comment',	'//',		"\n",
	  'string',	'"',		'"',
	  'string',	"'",		"'",
	  'include',	'#include',	"\n");


sub warning {
    print(STDERR "[",scalar(localtime),"] warning: $_[0]\n");
    print("<h4 align=\"center\"><i>** Warning: $_[0]</i></h4>\n") if $wwwdebug;
}


sub fatal {
    print(STDERR "[",scalar(localtime),"] fatal: $_[0]\n");
    print("<h4 align=\"center\"><i>** Fatal: $_[0]</i></h4>\n") if $wwwdebug;
    exit(1);
}


sub abortall {
    print(STDERR "[",scalar(localtime),"] abortall: $_[0]\n");
    print("Content-Type: text/html\n\n",
	  "<html>\n<head>\n<title>Abort</title>\n</head>\n",
	  "<body><h1>Abort!</h1>\n",
	  "<b><i>** Aborting: $_[0]</i></b>\n",
	  "</body>\n</html>\n") if $wwwdebug;
    exit(1);
}


sub fflush {
    $| = 1; print('');
}


sub urlargs {
    my @args = @_;
    my %args = ();
    my $val;

    foreach (@args) {
	$args{$1} = $2 if /(\S+)=(\S*)/;
    }
    @args = ();

    foreach ($Conf->allvariables) {
	$val = $args{$_} || $Conf->variable($_);
	push(@args, "$_=$val") unless ($val eq $Conf->vardefault($_));
	delete($args{$_});
    }

    foreach (keys(%args)) {
	push(@args, "$_=$args{$_}");
    }

    return($#args < 0 ? '' : '?'.join(';',@args));
}    


sub fileref {
    my ($desc, $path, $line, @args) = @_;
    return("<a href=\"source$path".
	   &urlargs(@args).
	   ($line > 0 ? "#L$line" : "").
	   "\"\>$desc</a>");
}


sub diffref {
    my ($desc, $path, $darg) = @_;

    ($darg,$dval) = $darg =~ /(.*?)=(.*)/;
    return("<a href=\"diff$path".
	   &urlargs(($darg ? "diffvar=$darg" : ""),
		    ($dval ? "diffval=$dval" : ""),
		    @args).
	   "\"\>$desc</a>");
}


sub idref {
    my ($desc, $id, @args) = @_;
    return("<a href=\"ident".
	   &urlargs(($id ? "i=$id" : ""),
		    @args).
	   "\"\>$desc</a>");
}


sub http_wash {
    my $t = shift;
    $t =~ s/\+/ /g;
    $t =~ s/\%([\da-f][\da-f])/pack("C", hex($1))/gie;

    # Paranoia check. Regexp-searches in Glimpse won't work.
    # if ($t =~ tr/;<>*|\`&$!#()[]{}:\'\"//) {

    # Should be sufficient to keep "open" from doing unexpected stuff.
    if ($t =~ tr/<>|\"\'\`//) {
	&abortall("Illegal characters in HTTP-parameters.");
    }
    
    return($t);
}


sub markspecials {
    $_[0] =~ s/([\&\<\>])/\0$1/g;
}


sub htmlquote {
    $_[0] =~ s/\0&/&amp;/g;
    $_[0] =~ s/\0</&lt;/g;
    $_[0] =~ s/\0>/&gt;/g;
}


sub freetextmarkup {
    $_[0] =~ s#((ftp|http)://\S*[^\s.])#<a href=\"$1\">$1</a>#g;
    $_[0] =~ s/(&lt;(.*@.*)&gt;)/<a href=\"mailto:$2\">$1<\/a>/g;
}


sub linetag {
#$frag =~ s/\n/"\n".&linetag($virtp.$fname, $line)/ge;
#    my $tag = '<a href="'.$_[0].'#L'.$_[1].
#	'" name="L'.$_[1].'">'.$_[1].' </a>';
    my $tag;
    $tag .= ' ' if $_[1] < 10;
    $tag .= ' ' if $_[1] < 100;
    $tag .= &fileref($_[1], $_[0], $_[1]).' ';
    $tag =~ s/<a/<a name=L$_[1]/;
#    $_[1]++;
    return($tag);
}


sub markupfile {
    my ($INFILE, $virtp, $fname, $outfun) = @_;

    $line = 1;

    # A C/C++ file 
    if ($fname =~ /\.([ch]|cpp?|cc)$/i) { # Duplicated in genxref.

	&SimpleParse::init($INFILE, @cterm);
	
	tie (%xref, "DB_File", $Conf->dbdir."/xref", O_RDONLY, 0664, $DB_HASH)
	    || &warning("Cannot open xref database.");

	&$outfun(# "<pre>\n".
		 #"<a name=\"L".$line++.'"></a>');
		 &linetag($virtp.$fname, $line++));

	($btype, $frag) = &SimpleParse::nextfrag;
	
	while (defined($frag)) {
	    &markspecials($frag);

	    if ($btype eq 'comment') {
		# Comment
		# Convert mail adresses to mailto:
		&freetextmarkup($frag);
		$frag = "<b><i>$frag</i></b>";
		$frag =~ s#\n#</i></b>\n<b><i>#g;
	    } elsif ($btype eq 'string') {
		# String
		$frag = "<i>$frag</i>";
		
	    } elsif ($btype eq 'include') { 
		# Include directive
		$frag =~ s#\"(.*)\"#
		    '"'.&fileref($1, $virtp.$1).'"'#e;
		$frag =~ s#&lt;(.*)&gt;#
		    "&lt;".&fileref
			($1, 
			 $Conf->mappath($Conf->incprefix."/$1")).
			     "&gt;"#e;
	    } else {
		# Code
		$frag =~ s#(^|[^a-zA-Z_\#0-9])([a-zA-Z_~][a-zA-Z0-9_]*)\b#
		    "$1".(defined($xref{$2}) ?
			  &idref($2,$2) :
			  "$2")#ge;
	    }

	    &htmlquote($frag);
	    $frag =~ s/\n/"\n".&linetag($virtp.$fname, $line++)/ge;
	    &$outfun($frag);
	    
	    ($btype, $frag) = &SimpleParse::nextfrag;
	}
	    
#	&$outfun("</pre>\n");
	untie(%xref);

    } elsif ($fname =~ /\.(gif|jpg)$/) {
	&$outfun("<img src=\"http:source".$virtp.$fname. &urlargs("raw=1").
		 "\" border=0 alt=\"$fname\" align=middle>\n");

    } elsif ($fname eq 'CREDITS') {
	while (<$INFILE>) {
	    &SimpleParse::untabify($_);
	    &markspecials($_);
	    &htmlquote($_);
	    s/^N:\s+(.*)/<hr>\n<h3>$1<\/h3>/gm;
	    s/^(E:\s+)(\S+@\S+)/$1<a href=\"mailto:$2\">$2<\/a>/gm;
	    s/^(W:\s+)(.*)/$1<a href=\"$2\">$2<\/a>/gm;
#	    &$outfun("<a name=\"L$.\"><\/a>".$_);
	    &$outfun(&linetag($virtp.$fname, $.).$_);
	}
    } else {
	while (<$INFILE>) {
	    &SimpleParse::untabify($_);
	    &markspecials($_);
	    &htmlquote($_);
	    &freetextmarkup($_);
#	    &$outfun("<a name=\"L$.\"><\/a>".$_);
	    &$outfun(&linetag($virtp.$fname, $.).$_);
	}
    }
}


sub fixpaths {
    $Path->{'virtf'} = '/'.shift;
    $Path->{'root'} = $Conf->sourceroot;
    
    while ($Path->{'virtf'} =~ s#/[^/]+/\.\./#/#g) {
       }
    $Path->{'virtf'} =~ s#/\.\./#/#g;
	   
    $Path->{'virtf'} .= '/' if (-d $Path->{'root'}.$Path->{'virtf'});
    $Path->{'virtf'} =~ s#//+#/#g;
    
    ($Path->{'virt'}, $Path->{'file'}) = $Path->{'virtf'} =~ m#^(.*/)([^/]*)$#;

    $Path->{'real'} = $Path->{'root'}.$Path->{'virt'};
    $Path->{'realf'} = $Path->{'root'}.$Path->{'virtf'};

    @pathelem = $Path->{'virtf'} =~ /([^\/]+$|[^\/]+\/)/g;
    
    $fpath = '';
    foreach (@pathelem) {
	$fpath .= $_;
	push(@addrelem, $fpath);
    }

    unshift(@pathelem, $Conf->sourcerootname.'/');
    unshift(@addrelem, "");
    
    foreach (0..$#pathelem) {
	if (defined($addrelem[$_])) {
	    $Path->{'xref'} .= &fileref($pathelem[$_], "/$addrelem[$_]");
	} else {
	    $Path->{'xref'} .= $pathelem[$_];
	}
    }
    $Path->{'xref'} =~ s#/</a>#</a>/#gi;
}


sub init {
    my @a;

    $HTTP->{'path_info'} = &http_wash($ENV{'PATH_INFO'});
    $HTTP->{'this_url'} = &http_wash(join('', 'http://',
					  $ENV{'SERVER_NAME'},
					  ':', $ENV{'SERVER_PORT'},
					  $ENV{'SCRIPT_NAME'},
					  $ENV{'PATH_INFO'},
					  '?', $ENV{'QUERY_STRING'}));

    foreach ($ENV{'QUERY_STRING'} =~ /([^;&=]+)(?:=([^;&]+)|)/g) {
	push(@a, &http_wash($_));
    }
    $HTTP->{'param'} = {@a};

    $HTTP->{'param'}->{'v'} ||= $HTTP->{'param'}->{'version'};
    $HTTP->{'param'}->{'a'} ||= $HTTP->{'param'}->{'arch'};
    $HTTP->{'param'}->{'i'} ||= $HTTP->{'param'}->{'identifier'};


    $identifier = $HTTP->{'param'}->{'i'};
    $readraw    = $HTTP->{'param'}->{'raw'};
    
    if (defined($readraw)) {
	print("\n");
    } else {
	print("Content-Type: text/html\n\n");
    }
    
    $Conf = new LXR::Config;
    
    foreach ($Conf->allvariables) {
	$Conf->variable($_, $HTTP->{'param'}->{$_}) if $HTTP->{'param'}->{$_};
    }
    
    &fixpaths($HTTP->{'path_info'} || $HTTP->{'param'}->{'file'});

    if (defined($readraw)) {
	open(RAW, $Path->{'realf'});
	while (<RAW>) {
	    print;
	}
	close(RAW);
	exit;
    }

    return($Conf, $HTTP, $Path);
}


sub expandtemplate {
    my ($templ, %expfunc) = @_;
    my ($expfun, $exppar);

    while ($templ =~ s/(\{[^\{\}]*)\{([^\{\}]*)\}/$1\01$2\02/s) {}
    
    $templ =~ s/(\$(\w+)(\{([^\}]*)\}|))/{
	if (defined($expfun = $expfunc{$2})) {
	    if ($3 eq '') {
		&$expfun;
	    } else {
		$exppar = $4;
		$exppar =~ s#\01#\{#gs;
		$exppar =~ s#\02#\}#gs;
		&$expfun($exppar);
	    }
	} else {
	    $1;
	}
    }/ges;

    $templ =~ s/\01/\{/gs;
    $templ =~ s/\02/\}/gs;
    return($templ);
}


# What follows is a pretty hairy way of expanding nested templates.
# State information is passed via localized variables.

# The first one is simple, the "banner" template is empty, so we
# simply return an appropriate value.
sub bannerexpand {
    if ($who eq 'source' || $who eq 'diff') {
	return($Path->{'xref'});
    } else {
	return('');
    }
}


sub titleexpand {
    if ($who eq 'source' || $who eq 'diff') {
	return($Conf->sourcerootname.$Path->{'virtf'});

    } elsif ($who eq 'ident') {
	my $i = $HTTP->{'param'}->{'i'};
	return($Conf->sourcerootname.' identfier search'.
	       ($i ? " \"$i\"" : ''));

    } elsif ($who eq 'search') {
	my $s = $HTTP->{'param'}->{'string'};
	return($Conf->sourcerootname.' freetext search'.
	       ($s ? " \"$s\"" : ''));

    } elsif ($who eq 'find') {
	my $s = $HTTP->{'param'}->{'string'};
	return($Conf->sourcerootname.' file search'.
	       ($s ? " \"$s\"" : ''));
    }
}


sub thisurl {
    my $url = $HTTP->{'this_url'};

    $url =~ s/([\?\&\;\=])/sprintf('%%%02x',(unpack('c',$1)))/ge;
    return($url);
}


sub baseurl {
    return($Conf->baseurl);
}

# This one isn't too bad either.  We just expand the "modes" template
# by filling in all the relevant values in the nested "modelink"
# template.
sub modeexpand {
    my $templ = shift;
    my $modex = '';
    my @mlist = ();
    local $mode;
    
    if ($who eq 'source') {
	push(@mlist, "<b><i>source navigation</i></b>");
    } else {
	push(@mlist, &fileref("source navigation", $Path->{'virtf'}));
    }
    
    if ($who eq 'diff') {
	push(@mlist, "<b><i>diff markup</i></b>");
	
    } elsif ($who eq 'source' && $Path->{'file'}) {
	push(@mlist, &diffref("diff markup", $Path->{'virtf'}));
    }
    
    if ($who eq 'ident') {
	push(@mlist, "<b><i>identifier search</i></b>");
    } else {
	push(@mlist, &idref("identifier search", ""));
    }

    if ($who eq 'search') {
	push(@mlist, "<b><i>freetext search</i></b>");
    } else {
	push(@mlist, "<a href=\"search".
	     &urlargs."\">freetext search</a>");
    }
    
    if ($who eq 'find') {
	push(@mlist, "<b><i>file search</i></b>");
    } else {
	push(@mlist, "<a href=\"find".
	     &urlargs."\">file search</a>");
    }
    
    foreach $mode (@mlist) {
	$modex .= &expandtemplate($templ,
				  ('modelink', sub { return($mode) }));
    }
    
    return($modex);
}

# This is where it gets a bit tricky.  varexpand expands the
# "variables" template using varname and varlinks, the latter in turn
# expands the nested "varlinks" template using varval.
sub varlinks {
    my $templ = shift;
    my $vlex = '';
    my ($val, $oldval);
    local $vallink;
    
    $oldval = $Conf->variable($var);
    foreach $val ($Conf->varrange($var)) {
	if ($val eq $oldval) {
	    $vallink = "<b><i>$val</i></b>";
	} else {
	    if ($who eq 'source') {
		$vallink = &fileref($val, 
				    $Conf->mappath($Path->{'virtf'},
						   "$var=$val"),
				    0,
				    "$var=$val");

	    } elsif ($who eq 'diff') {
		$vallink = &diffref($val, $Path->{'virtf'}, "$var=$val");
		
	    } elsif ($who eq 'ident') {
		$vallink = &idref($val, $identifier, "$var=$val");
		
	    } elsif ($who eq 'search') {
		$vallink = "<a href=\"search".
		    &urlargs("$var=$val",
			     "string=".$HTTP->{'param'}->{'string'}).
				 "\">$val</a>";
		
	    } elsif ($who eq 'find') {
		$vallink = "<a href=\"find".
		    &urlargs("$var=$val",
			     "string=".$HTTP->{'param'}->{'string'}).
				 "\">$val</a>";
	    }
	}
	$vlex .= &expandtemplate($templ,
				 ('varvalue', sub { return($vallink) }));

    }
    return($vlex);
}


sub varexpand {
    my $templ = shift;
    my $varex = '';
    local $var;
    
    foreach $var ($Conf->allvariables) {
	$varex .= &expandtemplate($templ,
				  ('varname',  sub { 
				      return($Conf->vardescription($var))}),
				  ('varlinks', \&varlinks));
    }
    return($varex);
}


sub makeheader {
    local $who = shift;

    if ($Conf->htmlhead && !open(TEMPL, $Conf->htmlhead)) {
	&warning("Template ".$Conf->htmlhead." does not exist.");
	$template ||= "<html><body>\n<hr>\n";
    } else {
	$save = $/; undef($/);
	$template = <TEMPL>;
	$/ = $save;
	close(TEMPL);
    }
    
    print(
#"<!doctype html public \"-//W3C//DTD HTML 3.2//EN\">\n",
#	  "<html>\n",
#	  "<head>\n",
#	  "<title>",$Conf->sourcerootname," Cross Reference</title>\n",
#	  "<base href=\"",$Conf->baseurl,"\">\n",
#	  "</head>\n",

    	  &expandtemplate($template,
			  ('title',	\&titleexpand),
			  ('banner',	\&bannerexpand),
			  ('baseurl',	\&baseurl),
			  ('thisurl',	\&thisurl),
    			  ('modes',	\&modeexpand),
    			  ('variables',	\&varexpand)));
}


sub makefooter {
    local $who = shift;

    if ($Conf->htmltail && !open(TEMPL, $Conf->htmltail)) {
	&warning("Template ".$Conf->htmltail." does not exist.");
	$template = "<hr>\n</body>\n";
    } else {
	$save = $/; undef($/);
	$template = <TEMPL>;
	$/ = $save;
	close(TEMPL);
    }
    
    print(&expandtemplate($template,
			  ('banner',	\&bannerexpand),
			  ('thisurl',	\&thisurl),
    			  ('modes',	\&modeexpand),
    			  ('variables',	\&varexpand)),
	  "</html>\n");
}


1;

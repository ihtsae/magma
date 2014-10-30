#!/usr/bin/perl
#
# Processes include/magma_[sdcz].h header files (pre-processed as magma_[sdcz].i)
# to generate
# control/magma_zf77.cpp     Fortran wrappers and
# control/magma_zfortran.F90 Fortran interfaces.
#
# @author Mark Gates

my $usage = "Usage: $0 [--opencl] [--wrapper file] [--interface file] magma_z.i\n";

use strict;
use Text::Balanced qw( extract_bracketed );
use Getopt::Long;
use Text::Wrap;

# --------------------
# declare constants and variables
my( $pre, $return, $func, $is_gpu, $text, $rest, $comment,
	$funcf,
	$wrapper, $call, $interface, $vars,
	$args, @args, $arg, $type, $type2, $base_type, $var, $first_arg, $is_ptr );

# ignore auxiliary functions which the user doesn't need
# ignore PLASMA functions (tstrf, incpiv)
# ignore misc functions in headers but not in library (larfg, getrf2, geqr2) (as of 2012-04-16)
my @ignore = qw(
	[sdcz]geqrf_msub
	[sdcz]getrf_msub
	[sdcz]getrf2_msub
	[sdcz]potrf_msub
	[sdcz]potrf2_msub
	
	[cz]lahef
	[sd]lasyf
	[sd]sygvr
	[sd]sygvx
	[sdcz]geqr2
	[sdcz]getrf2
	[sdcz]getrf_incpiv
	[sdcz]labrd
	[sdcz]laex\d
	[sdcz]laex\d_m
	[sdcz]lahr2
	[sdcz]lahr2_m
	[sdcz]lahru
	[sdcz]lahru_m
	[sdcz]laqps\d?
	[sdcz]larfg
	[sdcz]latrd
	[sdcz]latrd2
	[sdcz]tsqrt
	[sdcz]tstrf
);
my $ignore = join( "|", @ignore );
print STDERR "ignore: $ignore\n";

# map C types to name for magma_<name>_const(...) call
my %constants = (
	'magma_order_t'      => 'order'  ,
	'magma_trans_t'      => 'trans'  ,
	'magma_uplo_t'       => 'uplo'   ,
	'magma_diag_t'       => 'diag'   ,
	'magma_side_t'       => 'side'   ,
	'magma_type_t'       => 'type'   ,
	'magma_norm_t'       => 'norm'   ,
	'magma_dist_t'       => 'dist'   ,
	'magma_pack_t'       => 'pack'   ,
	'magma_vec_t'        => 'vec'    ,
	'magma_range_t'      => 'range'  ,
	'magma_vect_t'       => 'vect'   ,
	'magma_direct_t'     => 'direct' ,
	'magma_storev_t'     => 'storev' ,
	'magma_order_t'      => 'order'  ,
	'magma_trans_t'      => 'trans'  ,
	'magma_uplo_t'       => 'uplo'   ,
	'magma_diag_t'       => 'diag'   ,
	'magma_side_t'       => 'side'   ,
	'magma_type_t'       => 'type'   ,
	'magma_norm_t'       => 'norm'   ,
	'magma_dist_t'       => 'dist'   ,
	'magma_bool_t'       => 'bool'   ,
);

# map C base types to Fortran types
my %f90types = (
	'size_t'             => 'integer         ',  # dose Fortran have unsigned?
	'char'               => 'character       ',
	'int'                => 'integer         ',
	'magma_int_t'        => 'integer         ',
	'float'              => 'real            ',
	'double'             => 'double precision',
	'cuFloatComplex'     => 'complex         ',
	'cuDoubleComplex'    => 'complex*16      ',
	'magmaFloatComplex'  => 'complex         ',
	'magmaDoubleComplex' => 'complex*16      ',
	'magma_queue_t'      => 'integer         ',  # not sure what type to use for streams -- they are pointers, right?
	'magma_event_t'      => 'integer         ',  # not sure what type to use for events  -- they are pointers, right?
	
	'magmaFloat_ptr'         => 'magma_devptr_t',
	'magmaDouble_ptr'        => 'magma_devptr_t',
	'magmaFloatComplex_ptr'  => 'magma_devptr_t',
	'magmaDoubleComplex_ptr' => 'magma_devptr_t',
);

# add all symbolic constant types (magma_uplo_t, etc.), mapping to character
for my $type ( keys %constants ) {
	$f90types{ $type } = 'character       ';
}

my %devptrs = (
	'magma_int_t'            => 'magma_idevptr',
	'float'                  => 'magma_sdevptr',
	'double'                 => 'magma_ddevptr',
	
	'cuFloatComplex'         => 'magma_cdevptr',
	'cuDoubleComplex'        => 'magma_zdevptr',
	'magmaFloatComplex'      => 'magma_cdevptr',
	'magmaDoubleComplex'     => 'magma_zdevptr',
	
	'magmaFloat_ptr'         => 'magma_sdevptr',
	'magmaDouble_ptr'        => 'magma_ddevptr',
	'magmaFloatComplex_ptr'  => 'magma_cdevptr',
	'magmaDoubleComplex_ptr' => 'magma_zdevptr',
	
	'magmaFloat_const_ptr'         => 'magma_sdevptr',
	'magmaDouble_const_ptr'        => 'magma_ddevptr',
	'magmaFloatComplex_const_ptr'  => 'magma_cdevptr',
	'magmaDoubleComplex_const_ptr' => 'magma_zdevptr',
);

# Fortran 90 has 132 line length limit, so wrap text
$Text::Wrap::separator = "  &\n";
$Text::Wrap::columns   = 90;
$Text::Wrap::unexpand  = 0;  # no tabs


# --------------------
# parse options
my $verbose = 0;
my $opencl = 0;
my( $file_wrapper, $file_interface );
GetOptions(
	"verbose"     => \$verbose,
	"opencl"      => \$opencl,
	"wrapper=s"   => \$file_wrapper,
	"interface=s" => \$file_interface,
) or die( "$!$usage" );

print "ARGV: @ARGV\n";

if ( $#ARGV != 0 ) {
	die("Specify one header file as input.\n$usage");
}

my( $precision ) = $ARGV[0] =~ m/magma_([sdcz])\.i/;
if ( not $precision ) {
	die("Specify one of magma_[sdcz].i pre-processed header files.\n$usage");
}

# default output files
if ( not $file_wrapper   ) { $file_wrapper   = "magma_${precision}f77.cpp"; }
if ( not $file_interface ) { $file_interface = "magma_${precision}fortran.F90"; }

print "file_wrapper   $file_wrapper\n";
print "file_interface $file_interface\n";


# --------------------
# header for magma_zf77.cpp wrappers
my $output_wrapper = <<EOT;
/*******************************************************************************
 *  This file is AUTOMATICALLY GENERATED by:
 *  $0 @ARGV
 *  Do not edit.
 ******************************************************************************/

#include <stdint.h>  // for uintptr_t

#include "magma.h"
#include "magma_mangling.h"

/*
 * typedef comming from fortran.h file provided in CUDADIR/src directory
 * it will probably change with future release of CUDA when they use 64 bit addresses
 */
typedef size_t devptr_t;

#ifdef PGI_FORTRAN
    #define magma_idevptr(ptr_) ((int*)               (ptr_))
    #define magma_zdevptr(ptr_) ((magmaDoubleComplex*)(ptr_))
    #ifndef magma_cdevptr
    #define magma_cdevptr(ptr_) ((magmaFloatComplex*) (ptr_))
    #endif
    #ifndef magma_ddevptr
    #define magma_ddevptr(ptr_) ((double*)            (ptr_))
    #endif
    #ifndef magma_sdevptr
    #define magma_sdevptr(ptr_) ((float*)             (ptr_))
    #endif
#else
    #define magma_idevptr(ptr_) ((int*)               (uintptr_t)(*(ptr_)))
    #define magma_zdevptr(ptr_) ((magmaDoubleComplex*)(uintptr_t)(*(ptr_)))
    #ifndef magma_cdevptr
    #define magma_cdevptr(ptr_) ((magmaFloatComplex*) (uintptr_t)(*(ptr_)))
    #endif
    #ifndef magma_ddevptr
    #define magma_ddevptr(ptr_) ((double*)            (uintptr_t)(*(ptr_)))
    #endif
    #ifndef magma_sdevptr
    #define magma_sdevptr(ptr_) ((float*)             (uintptr_t)(*(ptr_)))
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

EOT


# --------------------
# header for magma_zfortran.F90 interfaces
my $output_interface = <<EOT;
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!  This file is AUTOMATICALLY GENERATED by:
!!  $0 @ARGV
!!  Do not edit.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

module magma_${precision}fortran

use magma_param

implicit none

!---- Fortran interfaces to MAGMA subroutines ----
interface

EOT


# --------------------
undef $/;  # slurp whole file
$_ = <>;

# strip out everything preceding the last extern "C", and the extern "C" itself.
# This is our crude method to tell what is magma stuff and what is from other included headers.
s/^.*extern "C" \{\s*//s;
s/\}\s*$//;

# strip out pre-processor directives (i.e., include file line numbers)
s/^#.*//mg;

# strip out lines we don't want to copy
s/void \wpanel_to_q.*\n//;
s/void \wq_to_panel.*\n//;

# strip out extern consts
s/extern const .*;\n//g;


while( $_ ) {
	# look for function, e.g.: "magma_int_t magma_foo_gpu("
	if ( m/(.*?)^(magma_int_t|int|void)\s+magma_(\w+?)(_gpu)?\s*(\(.*)/ms ) {
		# parse magma function
		$pre    = $1;
		$return = $2;
		$func   = $3;
		$is_gpu = $4;
		$text   = $5;
		
		# get arguments: "( ... )"
		($args, $rest) = extract_bracketed( $text, '()' );
		$args =~ s/\n/ /g;
		$args =~ s/^\( *//;
		$args =~ s/ *\)$//;
		@args = split( /, */, $args );
		
		$funcf = "magmaf_$func$is_gpu";
		if ( $is_gpu ) {
			$wrapper = sprintf( "#define %s FORTRAN_NAME( %s, %s )\n",
				${funcf}, $funcf, uc($funcf) );
		}
		else {
			$wrapper = sprintf( "#define %s FORTRAN_NAME( %s, %s )\n",
				${funcf}, $funcf, uc($funcf) );
		}
		
		my $match = $func =~ m/^($ignore)$/;
		if ( $verbose ) {
			print STDERR "FUNC $func $match\n";
		}
		if ( $opencl and $is_gpu ) {
			# ignore OpenCL GPU functions, since
			# we haven't dealt with passing cl_mem objects in Fortran yet
			$wrapper   = "";
			$interface = "";
		}
		elsif ( $func =~ m/^($ignore)$/ or $func =~ m/_mgpu/ ) {
			# ignore auxiliary functions and multi-GPU functions, since
			# we haven't dealt with passing arrays of pointers in Fortran yet
			$wrapper   = "";
			$interface = "";
		}
		elsif ( $func =~ m/get_\w+_nb/ and $#args == 0 ) {  # i.e., len(@args) is 1
			# special case for get_nb functions, to return a value
			# is returning an int safe? otherwise, we could make these take an output argument.
			$wrapper  .= "magma_int_t ${funcf}( magma_int_t *m )\n{\n    return magma_$func( *m );\n}\n\n";
			$interface = "integer function $funcf( m )\n    integer :: m\nend function $funcf\n\n";
		}
		else {
			# build up wrapper and the call inside the wrapper, argument by argument
			$wrapper .= "void ${funcf}(\n    ";
			$call     = "magma_$func$is_gpu(\n        ";
			
			# build up Fortran interface and variable definitions, argument by argument
			$interface = "subroutine $funcf( ";
			$vars      = "";
			
			$first_arg = 1;
			foreach $arg ( @args ) {
				($type, $var, $type2) = $arg =~ m/^((?:const +)?\w+(?: *\*+)?) *(\w+)([\[0-9\]]*)$/;
				if ( not $type ) {
					print STDERR "\nFAILED: func $func, arg $arg\n";
				}
				$base_type = $type;
				$base_type =~ s/\*//;
				$base_type =~ s/const +//;
				$base_type =~ s/^ +//;
				$base_type =~ s/ +$//;
				#print STDERR "base_type $base_type\n";
				
				$is_ptr = ($type =~ m/\*/ or $type =~ m/_ptr$/ or $type2 ne "");
				#print STDERR "  type $type, type2 $type2, var $var, is_ptr $is_ptr\n";
				if ( $is_ptr ) {
					unless( $first_arg ) {
						$wrapper   .= ",\n    ";
						$call      .= ",\n        ";
						$interface .= ", ";
					}
					if ( $type =~ m/_ptr$/ or ($is_gpu and $var =~ m/^d\w+/) or $var eq "dT" ) {
						# Now, the _ptr types identify device pointers.
						# Previously, for _gpu interfaces, assume ptrs that start with "d" are device pointers.
						# Also CPU interface for geqrf, etc., passes dT as device pointer (weirdly).
						if ( $type !~ m/_ptr$/ ) {
							print STDERR "WARNING: assuming in $func$is_gpu: $var is devptr, but lacks _ptr!\n";
						}
						if ( not defined $devptrs{$base_type} ) {
							print STDERR "ERROR: devptrs{$base_type} not defined\n";
						}
						$wrapper .= "devptr_t *$var";
						$call    .= "$devptrs{$base_type}($var)";
						$vars    .= "    magma_devptr_t   :: $var\n";
					}
					else {
						$wrapper .= $arg;
						$call    .= $var;
						if ( $var =~ m/^(info|iter|m)$/ ) {
							# special case for variables passed as pointers in C,
							# but are single values, not arrays.
							# e.g., see zhegvd and zcgesv
							$vars .= "    $f90types{$base_type} :: $var\n";
						}
						else {
							$vars .= "    $f90types{$base_type} :: $var(*)\n";
						}
					}
					$interface .= $var;
				}
				else {
					unless( $first_arg ) {
						$wrapper   .= ", ";
						$call      .= ", ";
						$interface .= ", ";
					}
					# convert scalars to pointers for Fortran interface
					# map string constants => symbolic constants
					if ( $constants{$type} ) {
						$wrapper   .= "const char* $var";
						$call      .= "magma_$constants{$type}_const(*$var)";
					}
					else {
						$wrapper   .= "$type *$var";
						$call      .= "*$var";
					}
					$interface .= $var;
					$vars      .= "    $f90types{$base_type} :: $var\n";
				}
				$first_arg = 0;
			}
			$wrapper .= " )\n{\n    $call );\n}\n\n";
			
			$interface .= " )\n";
			$interface  = Text::Wrap::wrap( "", "        ", $interface );
			$interface .= "${vars}end subroutine $funcf\n\n";
		}
		
		if ( $pre and $verbose ) {
			print STDERR "WARNING: ignoring unrecognized text before function: <<<\n$pre>>>\n";
		}
		
		$output_wrapper   .= $wrapper;
		$output_interface .= $interface;
		
		$_ = $rest;
		s/^ *;//;
	}
	else {
		if ( $verbose ) {
			print STDERR "WARNING: ignoring unrecognized text at end of file: <<<\n$_>>>\n";
		}
		last;
	}
	s/^\n+//;
}


# --------------------
# footer for magma_zf77.cpp wrappers
$output_wrapper .= <<EOT;

#ifdef __cplusplus
}
#endif
EOT


# --------------------
# footer for magma_zfortran.F90 interfaces
my %datatype = (
	s => 'real',
	d => 'double',
	c => 'complex',
	z => 'complex_16',
);

$output_interface .= <<EOT;
end interface

!---- Fortran-only subroutines (see $0 to edit) ----
contains

subroutine magmaf_${precision}off1d( ptrNew, ptrOld, inc, i)
    magma_devptr_t   :: ptrNew
    magma_devptr_t   :: ptrOld
    integer          :: inc, i

    ptrNew = ptrOld + (i-1) * inc * sizeof_$datatype{$precision}
end subroutine magmaf_${precision}off1d

subroutine magmaf_${precision}off2d( ptrNew, ptrOld, lda, i, j)
    magma_devptr_t   :: ptrNew
    magma_devptr_t   :: ptrOld
    integer          :: lda, i, j

    ptrNew = ptrOld + ((j-1) * lda + (i-1)) * sizeof_$datatype{$precision}
end subroutine magmaf_${precision}off2d

end module magma_${precision}fortran
EOT


# --------------------
# output both files
open( OUT, ">$file_wrapper" ) or die( "Can't open $file_wrapper: $!" );
print OUT $output_wrapper;
close( OUT );

open( OUT, ">$file_interface" ) or die( "Can't open $file_interface: $!" );
print OUT $output_interface;
close( OUT );

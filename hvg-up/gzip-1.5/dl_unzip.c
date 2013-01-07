#include <stdio.h>
#include <string.h>
#include "gzip.h"

#define FILE_LENTH	30

static struct stat istat;         /* status for input file */
int  ifd;                  /* input file descriptor */
int  ofd;                  /* output file descriptor */

void treat_file(char *iname);
static int (*work) (int infile, int outfile) = unzip; /* function to call */

int main(int argc, char *argv)
{
	char infile[FILE_LENTH]="hello.tar.gz";
	treat_file(infile);

	return 0;

}




/* ========================================================================
 * Compress or decompress the given file
 */
local void treat_file(iname)
    char *iname;
{
    /* Accept "-" as synonym for stdin */
    if (strequ(iname, "-")) {
        int cflag = to_stdout;
        treat_stdin();
        to_stdout = cflag;
        return;
    }

    /* Check if the input file is present, set ifname and istat: */
    ifd = open_input_file (iname, &istat);
    if (ifd < 0)
      return;

    if (! to_stdout)
      {
        if (! S_ISREG (istat.st_mode))
          {
            WARN ((stderr,
                   "%s: %s is not a directory or a regular file - ignored\n",
                   program_name, ifname));
            close (ifd);
            return;
          }
        if (istat.st_mode & S_ISUID)
          {
            WARN ((stderr, "%s: %s is set-user-ID on execution - ignored\n",
                   program_name, ifname));
            close (ifd);
            return;
          }
        if (istat.st_mode & S_ISGID)
          {
            WARN ((stderr, "%s: %s is set-group-ID on execution - ignored\n",
                   program_name, ifname));
            close (ifd);
            return;
          }

        if (! force)
          {
            if (istat.st_mode & S_ISVTX)
              {
                WARN ((stderr,
                       "%s: %s has the sticky bit set - file ignored\n",
                       program_name, ifname));
                close (ifd);
                return;
              }
            if (2 <= istat.st_nlink)
              {
                WARN ((stderr, "%s: %s has %lu other link%c -- unchanged\n",
                       program_name, ifname,
                       (unsigned long int) istat.st_nlink - 1,
                       istat.st_nlink == 2 ? ' ' : 's'));
                close (ifd);
                return;
              }
          }
      }

    ifile_size = S_ISREG (istat.st_mode) ? istat.st_size : -1;
    time_stamp.tv_nsec = -1;
    if (!no_time || list)
      time_stamp = get_stat_mtime (&istat);

    /* Generate output file name. For -r and (-t or -l), skip files
     * without a valid gzip suffix (check done in make_ofname).
     */
    if (to_stdout && !list && !test) {
        strcpy(ofname, "stdout");

    } else if (make_ofname() != OK) {
        close (ifd);
        return;
    }

    clear_bufs(); /* clear input and output buffers */
    part_nb = 0;

    if (decompress) {
        method = get_method(ifd); /* updates ofname if original given */
        if (method < 0) {
            close(ifd);
            return;               /* error message already emitted */
        }
    }
    if (list) {
        do_list(ifd, method);
        if (close (ifd) != 0)
          read_error ();
        return;
    }

    /* If compressing to a file, check if ofname is not ambiguous
     * because the operating system truncates names. Otherwise, generate
     * a new ofname and save the original name in the compressed file.
     */
    if (to_stdout) {
        ofd = fileno(stdout);
        /* Keep remove_ofname_fd negative.  */
    } else {
        if (create_outfile() != OK) return;

        if (!decompress && save_orig_name && !verbose && !quiet) {
            fprintf(stderr, "%s: %s compressed to %s\n",
                    program_name, ifname, ofname);
        }
    }
    /* Keep the name even if not truncated except with --no-name: */
    if (!save_orig_name) save_orig_name = !no_name;

    if (verbose) {
        fprintf(stderr, "%s:\t", ifname);
    }

    /* Actually do the compression/decompression. Loop over zipped members.
     */
    for (;;) {
        if ((*work)(ifd, ofd) != OK) {
            method = -1; /* force cleanup */
            break;
        }

        if (input_eof ())
          break;

        method = get_method(ifd);
        if (method < 0) break;    /* error message already emitted */
        bytes_out = 0;            /* required for length check */
    }

    if (close (ifd) != 0)
      read_error ();

    if (!to_stdout)
      {
        sigset_t oldset;
        int unlink_errno;

        copy_stat (&istat);
        if (close (ofd) != 0)
          write_error ();

        sigprocmask (SIG_BLOCK, &caught_signals, &oldset);
        remove_ofname_fd = -1;
        unlink_errno = xunlink (ifname) == 0 ? 0 : errno;
        sigprocmask (SIG_SETMASK, &oldset, NULL);

        if (unlink_errno)
          {
            WARN ((stderr, "%s: ", program_name));
            if (!quiet)
              {
                errno = unlink_errno;
                perror (ifname);
              }
          }
      }

    if (method == -1) {
        if (!to_stdout)
          remove_output_file ();
        return;
    }

    /* Display statistics */
    if(verbose) {
        if (test) {
            fprintf(stderr, " OK");
        } else if (decompress) {
            display_ratio(bytes_out-(bytes_in-header_bytes), bytes_out,stderr);
        } else {
            display_ratio(bytes_in-(bytes_out-header_bytes), bytes_in, stderr);
        }
        if (!test && !to_stdout) {
            fprintf(stderr, " -- replaced with %s", ofname);
        }
        fprintf(stderr, "\n");
    }
}

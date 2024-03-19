/*
 * Cap - Command-line argument parser library in single header file.
 * Author: Hossein Khosravi (https://github.com/thehxdev)
 * Repository: https://github.com/thehxdev/cap
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef __CAP_H__
#define __CAP_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define CAP_VERSION "1.0.5"
#define CAP_DEINIT __attribute__((cleanup(cap_deinit)))


typedef struct Cap_RawArgs_t {
    char **argv;
    int argc;
} Cap_RawArgs_t;

typedef struct Cap_t Cap_t;


/* initialize cap */
Cap_t *cap_init(int argc, char **argv);


/* free all memory allocated by cap */
void cap_deinit(Cap_t **cap);


/* register a new sub-command */
int cap_register_subcmd(Cap_t *cap,
                        const char *name,
                        const char *help);


/* register a new flag 
 *
 * @subcmd can be NULL */
int cap_register_flag(Cap_t *cap,
                      const char *subcmd,
                      const char *name,
                      const char *help);


/* get a flag's value after parsing arguments
 *
 * @subcmd can be NULL */
char *cap_flag_getval(Cap_t *cap,
                      const char *subcmd,
                      const char *name);


/* is a flag present in arguments or not */
int cap_flag_provided(Cap_t *cap,
                      const char *subcmd,
                      const char *name);


/* get provided sub-command in command-line */
int cap_subcmd_provided(Cap_t *cap, const char *subcmd);


/* get sub-command's arguments as raw arguments */
Cap_RawArgs_t cap_subcmd_rawargs(Cap_t *cap, const char *subcmd);


/* parse the command-line arguments */
int cap_parse_args(Cap_t *cap);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __CAP_H__ */

#ifndef	_oacontract_h
#define	_oacontract_h

#ifndef _rest_h
struct rest_client;
struct rest_response;
#endif // _rest_h

struct cords_onapp_extras_handle
{
  struct cords_onapp_extras *ponapp_extras;
  int owner;
};

struct	oa_subscription 
{
	char *	name;
	char *	user;
	char *	password;
	char *	host;
	int	port;
	char *	tls;
	char * version;
};

struct rest_response *  oaprocci_alert_relay(
		void * i,
		struct rest_client * prest_client,
		struct rest_response * prest_response,
		int status, char * message,
		char * nature,
		char * agent, char * tls);

private	struct	oa_config * resolve_oa_configuration( char * sptr );
private	struct oa_config * use_onapp_configuration( char * nptr );
struct cords_onapp_extras_handle resolve_cords_onapp_extras_handle( char * sptr, char *agent, char *tls );
struct cords_onapp_extras_handle use_cords_onapp_extras_handle( char * sptr, char *agent, char *tls );
void close_cords_onapp_extras_handle(struct cords_onapp_extras_handle *handle);

	/* ------------- */
#endif 	/* _oacontract_h */
	/* ------------- */




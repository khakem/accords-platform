#ifndef	_ldapclient_c
#define	_ldapclient_c

#include "ldapclient.h"

/*	------------------------------------------	*/
/*		l d a p _ f a i l u r e			*/
/*	------------------------------------------	*/
private	struct ldap_controller * ldap_failure(struct ldap_controller * h, int ecode)
{
	char *	sptr;
	int	whoops;
	char *	mptr="default message";
#ifdef	LDAP_OPT_RESULT_CODE
	if ( ecode == -1 )
		if ( h )
			whoops = ldap_get_option( h->handle,LDAP_OPT_RESULT_CODE , &ecode);
#endif
#ifdef	LDAP_OPT_DIAGNOSTIC_MESSAGE
	whoops = ldap_get_option( h->handle,LDAP_OPT_DIAGNOSTIC_MESSAGE , &mptr);
#endif
	if ( h ) 
	{
		if ( h->handle ) 
		{
			ldap_unbind( h->handle );
			h->handle = (LDAP *) 0;
		}

		if ( h->atbvalue ) 
		{
			liberate( h->atbvalue );
			h->atbvalue = (void*) 0;
		}
		if ( h->tlsconf )
		{
			h->tlsconf = release_tls_configuration(h->tlsconf );
		}

		if ( h->host ) 
			h->host = liberate( h->host );
		if ( h->credentials ) 
			h->credentials = liberate( h->credentials );
		if ( h->password ) 
			h->password = liberate( h->password );
		if ( h->tls ) 
			h->tls = liberate( h->tls );
		if (!( ecode ))
			liberate( h );
	}
	if ( ecode )
		if ( check_verbose() )
			printf("   LDAP FAIL %s %u \n",mptr,ecode );
	close_socket_catcher(0,"ldap failure",ecode);
	return((struct ldap_controller * ) 0);
}

/*	------------------------------------------	*/
/*	   B u i l d S t r i n g M a t r i x 		*/
/*	------------------------------------------	*/
private	char ** BuildStringMatrix( char * vptr )
{
	int	i;
	int	pipecount=0;
	char *	sptr;
	char **	rptr;
	char ** root;
	for (	sptr=vptr; *sptr != 0; sptr++ )
		if ( *sptr == '|' )
			pipecount++;
	pipecount++;
	
	if (!( root = allocate( ((pipecount+1) * sizeof( char * )) ) ))
		return( root );
	else if (!( vptr = allocate_string( vptr ) )) 
	{
		liberate( root );
		return( (char **) 0 );
	}
	else	
	{
		for (	sptr=vptr, rptr=root; 
			pipecount != 0; rptr++ ) 
		{
			pipecount--;
			if (!( pipecount ))
				*rptr = allocate_string( sptr );
			else	
			{
				for ( i=0; *(sptr+i) != 0; i++ )
					if ( *(sptr+i) == '|' )
						break;
				*(sptr+i) = 0;
				*rptr = allocate_string(sptr);
				sptr += (i+1);
			}
		}
		*rptr = (char *) 0;
		liberate( vptr );
		return( root );
	}
}

/*	------------------------------------------	*/
/*		A d d L d a p M o d i f 		*/
/*	------------------------------------------	*/
private	void	AddLdapModif(struct ldap_controller * lptr, LDAPMod * mptr )
{
	LDAPMod ** nptr;
	LDAPMod ** optr;
	LDAPMod ** wptr;
	lptr->pending++;
	if (!( wptr = allocate( (lptr->pending + 1) * sizeof( LDAPMod * ) ) ))
		return;
	else if (( optr = lptr->modifications ) != (LDAPMod **) 0) 
	{
		for (nptr=wptr; *optr != (LDAPMod *) 0; optr++, nptr++ )
			*nptr = *optr;
		liberate( lptr->modifications );
	}
	else	nptr = wptr;
	*(nptr++) = mptr;
	*nptr = (LDAPMod *) 0;
	lptr->modifications = wptr;
	return;
}

/*	------------------------------------------	*/
/*		D r o p U p d a t e			*/
/*	------------------------------------------	*/
private	void	DropUpdate( 	LDAPMod	* mptr		)
{
	char **	sptr;
	
	if ( mptr ) 
	{

		if ( mptr->mod_type )
			liberate( mptr->mod_type );

		if ((sptr = mptr->mod_vals.modv_strvals) != (char **) 0) 
		{
			while ( *sptr != (char *) 0 )
				liberate( *(sptr++) );
			liberate( mptr->mod_vals.modv_strvals );
		}
		liberate( mptr );
	}
	return;
}

/*	---------------------------------------		*/
/*		O p e n L d a p 			*/
/*	---------------------------------------		*/
public	struct ldap_controller * OpenLdap( char * host, int port, char * credentials, char * password , char * tls )
{
	struct ldap_controller * lptr;
	LDAP	*	handle=(LDAP *) 0;
	int		mid=0;
	LDAP	*	ldap_open( const char *, int );

	if (!( lptr = allocate( sizeof( struct ldap_controller ) ) ))
		return( lptr );
	else if (!( lptr->host = allocate_string( host ) ))
		return( ldap_failure( lptr, 0 ) );
	else if (!( lptr->credentials = allocate_string( credentials ) ))
		return( ldap_failure( lptr, 0 ) );
	else if (!( lptr->password = allocate_string( password ) ))
		return( ldap_failure( lptr, 0 ) );
	{
		lptr->port 	= port;
		lptr->handle 	= (LDAP *) 0;
		lptr->message 	= 
		lptr->result 	= (LDAPMessage *) 0;
		lptr->domain	=
		lptr->dn	=
		lptr->atbname	= 
		lptr->atbvalue	= (char *) 0;
		lptr->synchronise = 1;
		lptr->operation = 
		lptr->pending	= 
		lptr->error	=
		lptr->messages  = 0;
#ifdef	LDAP_OPT_RESULT_CODE
		lptr->version   = LDAP_VERSION3;
#else
		lptr->version   = LDAP_VERSION2;
#endif
		lptr->itember	=(BerElement *) 0;
		lptr->server    =	
		lptr->client    = (LDAPControl *) 0;
		lptr->tlsmode   = LDAP_OPT_X_TLS_NEVER;
		lptr->keyfile   = 
		lptr->certfile  = (char *) 0;
		if ( rest_valid_string( tls ) )
			lptr->tls = allocate_string( tls );
		else	lptr->tls = (char *) 0;
		lptr->modifications = (LDAPMod**) 0;
		sprintf(lptr->url,"%s:%u",lptr->host,lptr->port);
		if (( rest_valid_string( lptr->tls ) )
 		&&  (( lptr->tlsconf = tls_configuration_load( lptr->tls )) != (struct tls_configuration *) 0))
		{		
			lptr->keyfile = lptr->tlsconf->key;
			lptr->certfile = lptr->tlsconf->certificate;
		}
		else	lptr->tlsconf = (struct tls_configuration *) 0; 
	}

	start_socket_catcher(0,"ldap open");
	if (!( lptr->host )) 
		return( ldap_failure( lptr, 0  ) );

	else if (!( lptr->credentials ))
		return( ldap_failure( lptr, 0  ) );

	else if (!( lptr->password )) 
		return( ldap_failure( lptr, 0  ) );

	if ( check_verbose() )
		printf("   LDAP OPEN %s : %u \n",lptr->host,lptr->port);

	if (!( lptr->handle = ldap_open( lptr->host, lptr->port )))
		return( ldap_failure(lptr,0) );

	if ( check_verbose() )
		printf("   LDAP SET OPTION: PROTOCOL VERSION %u\n",lptr->version);

	if ((lptr->error = ldap_set_option( lptr->handle, LDAP_OPT_PROTOCOL_VERSION, &lptr->version)) != LDAP_SUCCESS)
		return( ldap_failure(lptr,lptr->error) );

	if ( lptr->port == LDAPS_PORT )
	{
		if ( check_verbose() )
			printf("   LDAP SET OPTION: REQUIRE CERTIFICATE NEVER \n");

		if ((lptr->error = ldap_set_option( lptr->handle, LDAP_OPT_X_TLS_REQUIRE_CERT, &lptr->tlsmode)) != LDAP_SUCCESS)
			return( ldap_failure(lptr,lptr->error) );

		if ( check_verbose() )
			printf("   LDAP SET OPTION: TLS KEYFILE %s \n",lptr->keyfile);

		if ((lptr->error = ldap_set_option( lptr->handle, LDAP_OPT_X_TLS_KEYFILE, lptr->keyfile)) != LDAP_SUCCESS)
			return( ldap_failure(lptr,lptr->error) );

		if ( check_verbose() )
			printf("   LDAP SET OPTION: TLS CERTFILE %s \n",lptr->certfile);

		if ((lptr->error = ldap_set_option( lptr->handle, LDAP_OPT_X_TLS_CERTFILE, lptr->certfile)) != LDAP_SUCCESS)
			return( ldap_failure(lptr,lptr->error) );

		if ( check_verbose() )
			printf("   LDAP START TLS \n");

		if ((lptr->error = ldap_start_tls_s( lptr->handle, NULL, NULL )))
		{
			printf("ldap start tls -> %d:%u \n" ,lptr->error,errno);
			return( ldap_failure(lptr,lptr->error) );
		}
	}

	if ( check_verbose() )
		printf("   LDAP BIND '%s' : '%s' \n",lptr->credentials, lptr->password);

	if ((lptr->error = ldap_simple_bind_s( lptr->handle, lptr->credentials, lptr->password )) != LDAP_SUCCESS )
		return( ldap_failure(lptr,lptr->error) );

	else if ( is_socket_hit() )
		return( ldap_failure(lptr,lptr->error) );
	else
	{
		close_socket_catcher(0,"socket open",0);
		return( lptr );
	}
}

/*	-------------------------------		*/
/*		C l o s e L d a p 		*/
/*	-------------------------------		*/
public	void	CloseLdap( struct ldap_controller * lptr )
{
	if (!( lptr ))
		return;
	else	
	{
		start_socket_catcher(0,"ldap close");
		DropLdapUpdate( lptr );	
		ldap_failure( lptr, 0 );
	}
	return;
}

/*	--------------------------------------		*/
/*	L D A P   A T B   V A L U E ( handle )		*/
/*	--------------------------------------		*/
/*	Returns a pipe '|' separated list of		*/
/*	values for the current named atribut		*/
/*	positioned in the application structure		*/
/*	as the current atribut value			*/
/*	--------------------------------------		*/

public	 struct ldap_controller * LdapAtbValue(struct ldap_controller * lptr )
{
	int	i;
	char *	sptr;
	char *	rptr;
	char	**	vptr;

	if (!(vptr = ldap_get_values( lptr->handle, lptr->message, lptr->atbname)))
		return( lptr );
	else	
	{
		sptr = rptr = (char *) 0;
		for (	i=0; *(vptr+i) != (char *) 0; i++ ) 
		{
			if (!( rptr )) 
			{
				if (!( sptr = allocate_string( *(vptr+i) ) ))
					break;
			}
			else if (!( sptr = allocate( strlen( rptr ) + strlen( *(vptr+i) ) + 16 ) ))
				break;
			else	
			{
				sprintf(sptr,"%s|%s",rptr,*(vptr+i));
				liberate( rptr );
			}
			rptr = sptr;
			}
		ldap_value_free( vptr );
		lptr->atbvalue = rptr;
	}
	return( lptr );
}

/*	---------------------------------------------		*/
/*	F I R S T    L D A P   A T B ( handle, crit )		*/
/*	---------------------------------------------		*/

public	 struct ldap_controller * FirstLdapAtb(struct ldap_controller * lptr, char * criteria )
{
	lptr->message = lptr->result   = (LDAPMessage *) 0;
	lptr->atbname = lptr->atbvalue = (char *) 0;

	if (!( lptr->handle ))
		return( ldap_failure(lptr,lptr->error) );
	else if ((lptr->error = ldap_search_s(lptr->handle,criteria,LDAP_SCOPE_SUBTREE,"objectclass=*",NULL,0,&lptr->result)) != LDAP_SUCCESS)
		return( ldap_failure(lptr,lptr->error) );
	else if (!(lptr->messages = ldap_count_messages(lptr->handle, lptr->result)))
		return( lptr );
	else if (!(lptr->message = ldap_first_message(lptr->handle, lptr->result )))
		return( lptr );
	else if (!(lptr->dn = ldap_get_dn(lptr->handle,lptr->message)))
		return( lptr );
	else if (!(lptr->atbname = ldap_first_attribute( lptr->handle, lptr->message, & lptr->itember )))
		return( lptr );
	else 	return( LdapAtbValue( lptr ) );
}

/*	-------------------------------------		*/
/*	     N E X T    L D A P   A T B 		*/
/*	-------------------------------------		*/
public	 struct ldap_controller * NextLdapAtb(struct ldap_controller * lptr )
{
	if (!( lptr ))
		return( lptr );

	lptr->atbname = lptr->atbvalue = (char *) 0;

	if (!( lptr->message ))
		return( lptr );
	
	else if (!( lptr->handle ))
		return( ldap_failure(lptr,lptr->error) );
	else if (!(lptr->atbname = ldap_next_attribute( lptr->handle, lptr->message, lptr->itember )))
		return( lptr );

	else	return( LdapAtbValue( lptr ) );

}

/*	--------------------------------------------		*/
/*	D R O P     L D A P   U P D A T E ( handle )		*/
/*	--------------------------------------------		*/
public	 int	DropLdapUpdate(struct ldap_controller * lptr )
{
	LDAPMod	** mptr;
	if (( mptr = lptr->modifications) != (LDAPMod **) 0) 
	{
		for (	;
			*mptr != (LDAPMod *) 0;
			mptr++ ) 
		{
			DropUpdate( *mptr );
		}
	}

	if ( lptr->domain != (char *) 0 ) 
	{
		liberate( lptr->domain );
	}

	lptr->domain 		= (char *) 0;
	lptr->operation		=
	lptr->pending 		= 0;
	lptr->modifications 	= (LDAPMod**) 0;
	return(0);
}

/*	----------------------------------------------------		*/
/*		S T A R T   L D A P   U P D A T E 			*/
/*	----------------------------------------------------		*/
public	 int	StartLdapUpdate(struct ldap_controller * lptr, char * domain )
{
	if ( lptr->modifications ) 
	{ 
		DropLdapUpdate( lptr ); 
	}
	if ( domain ) 
	{
		lptr->domain = allocate_string( domain );
	}
	lptr->operation = LDAP_MOD_REPLACE;
	return(0);
}

/*	----------------------------------------------------		*/
/*		S T A R T   L D A P   D E L E T E 			*/
/*	----------------------------------------------------		*/
public	 int	StartLdapDelete(struct ldap_controller * lptr, char * domain )
{
	if ( lptr->modifications ) 
	{ 
		DropLdapUpdate( lptr ); 
	}
	if ( domain )
		lptr->domain = allocate_string( domain );
	lptr->operation = LDAP_MOD_DELETE;
	return(0);
}

/*	----------------------------------------------------		*/
/*		A D D  L D A P  S T R I N G  A T B 			*/
/*	----------------------------------------------------		*/
public	 int	AddLdapStringAtb(struct ldap_controller * lptr, char * vptr, char * nptr )
{
	LDAPMod	* mptr;
	if (!( nptr ))
		return( 30 );
	if (!( mptr = allocate( sizeof( LDAPMod ) ) ))
		return(27);
	else if (!( mptr->mod_type = allocate_string( nptr ) )) 
	{
		liberate( mptr );
		return( 27 );
	}
	else	
	{
		if ((mptr->mod_op = lptr->operation) != LDAP_MOD_DELETE) 
		{
			if ((!( vptr ))
			||  (!( *vptr))) vptr = " ";
			if ( vptr )
				mptr->mod_vals.modv_strvals = BuildStringMatrix(vptr);
			else	mptr->mod_vals.modv_strvals = (char **) 0;
		}
		else	mptr->mod_vals.modv_strvals = (char **) 0;
		AddLdapModif( lptr, mptr );
		return(0);
	}
}

/*	----------------------------------------------------		*/
/*		A D D  L D A P    W O R D    A T B 			*/
/*	----------------------------------------------------		*/
public	 int	AddLdapWordAtb(struct ldap_controller * lptr, int v, char * nptr )
{
	char	buffer[12];
	sprintf(buffer,"%u",v);
	return(AddLdapStringAtb(lptr, buffer, nptr ) );
}

/*	--------------------------------------------	*/
/*	     F L U S H   L D A P   U P D A T E 		*/
/*	--------------------------------------------	*/
public	 int	FlushLdapUpdate(struct ldap_controller * lptr )
{
	LDAPMod	** nptr;
	LDAPMod	* mptr;
	char * sptr;
	int	status;
	if (!( lptr ))
		return(0);
	else if (!( lptr->pending ))
		return(0);
	else if (!( lptr->domain ))
		return(0);
	else if (!( lptr->modifications ))
		return(0);
	else	
	{
		if ((lptr->error = ldap_modify_s( lptr->handle, lptr->domain, lptr->modifications )) != LDAP_SUCCESS ) 
		{
			if ( lptr->operation != LDAP_MOD_DELETE ) 
			{
				/* ------------------------------------------------------- */
				/* Not a delete operation so create the object if possible */
				/* ------------------------------------------------------- */
				if ( lptr->error == LDAP_NO_SUCH_OBJECT ) 
				{
					if (( nptr = lptr->modifications ) != (LDAPMod **) 0)
						while ((mptr = *(nptr++)) != (LDAPMod *) 0)
							mptr->mod_op = LDAP_MOD_ADD;
					if ((lptr->error = ldap_add_s( lptr->handle, lptr->domain, lptr->modifications )) != LDAP_SUCCESS) 
					{
						sptr = ldap_err2string( lptr->error );
						printf( "ldap error dn=%s: %s , %s\r\n",lptr->domain,sptr,"ldap_add_s");
						status = 66;
					}
				}
				else if ( lptr->error != LDAP_SUCCESS ) 
				{
					sptr = ldap_err2string( lptr->error );
					printf( "ldap error dn=%s: %s , %s\r\n",lptr->domain,sptr,"ldap_modify_s");
					status = 66;
				}
				else	status = 0;
			}
			else if ( lptr->error != LDAP_NO_SUCH_OBJECT ) 
			{
				sptr = ldap_err2string( lptr->error );
				printf( "ldap error dn=%s: %s , %s\r\n",lptr->domain,sptr,"ldap_modify_s(delete)");
				status = 0;
			}
			else	status = 0;
		}
		else	status = 0;
		DropLdapUpdate( lptr );
		return(status);
	}
}

/*	------------------------------------	*/
/*		D e l e t e L d a p 		*/
/*	------------------------------------	*/
public	int	DeleteLdap( struct ldap_controller * lptr, char * criteria )
{
	int	e;
	char *	sptr;

	if ((e = ldap_delete_s( lptr->handle, criteria )) != LDAP_SUCCESS ) 
	{
		sptr = ldap_err2string( e );
		printf("\nerror: ldap_delete_s: %u: %s \n",e,sptr);
		return( e );
	}
	else 	return(0);
}
	/* ------------- */
#endif	/* _ldapclient_c */
	/* ------------- */



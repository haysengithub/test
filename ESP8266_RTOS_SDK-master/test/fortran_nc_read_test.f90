program read
     implicit none 
     include '/public/software/netcdf-4.3.0/include/netcdf.inc'

     integer  ::  i,j,ncid,status
     integer  ::  ANOMid
   !  real*4     ::  x,y
     real     ::  ANOM(1388,584)


!!!!===============test nc read file=====================
     !status=nf_open('/home/liudi/fortran_test/SM_SCIE_MIR_CLF4DA_20101223T000000_20101223T235959_300_001_9.DBL.nc', nf_write, ncid)
     status=nf_open('/home/lincystar/nf90/SM_SCIE_MIR_CLF4DA_20101223T000000_20101223T235959_300_001_9.DBL.nc', nf_write, ncid)
	 
     if(status /= nf_noerr) call handle_err(status)

     status=nf_inq_varid(ncid, 'ANOM', ANOMid) 
     if(status /= nf_NoErr) call handle_err(status)
     status=nf_get_var_real(ncid, ANOMid, ANOM)
     if(status /= nf_NoErr) call handle_err(status)
     
     status=nf_close(ncid)
     if(status /= nf_NoErr) call handle_err(status)
    
  
open (1, file='ANOM.dat',form='unformatted',access='direct',recl=1388*584)
      write(1,rec=1) ((ANOM(i,j),i=1,1388),j=1,584)
close(1)

write(*,*) 'All done'

end program read
subroutine handle_err(status)
  integer status
  print *, nf_strerror(status)
  stop 'stopped'
end subroutine
  


   

#include "demultiplexer.h"

int reacotr::request_event(handle_t handle,event_t evt)
{
	epoll_event ep_evt;
	        ep_evt.data.fd = handle;
	        ep_evt.events = 0;

	        if (evt & EV_READ)
	        {
	            ep_evt.events |= EPOLLIN;
	        }
	        if (evt & EV_WRITE)
	        {
	            ep_evt.events |= EPOLLOUT;
	        }
	        ep_evt.events |= EPOLLONESHOT;

	        if (epoll_ctl(m_epoll_fd, EPOLL_CTL_MOD, handle, &ep_evt) != 0)
	        {
	            if (errno == ENOENT)
	            {
	                if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, handle, &ep_evt) != 0)
	                {
	                    return -errno;
	                }
	                ++m_fd_num;
	            }
	        }
	        return 0;
}

int reactor::unrequest_event(handle_t handle)
{
	 epoll_event ep_evt;
	if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, handle, &ep_evt) != 0)
	 {
	     return -errno;
	 }
	--m_fd_num;
	 return 0;
}

int reactor::wait_event(std::map<handle_t,event_handler *> *handlers_map,int timeout)
{
	std::vector<epoll_event> ep_evts(m_fd_num);
	int num = epoll_wait(m_epoll_fd, &ep_evts[0], ep_evts.size(), timeout);
	if(num>0)
	{
		for(idx=0;idx<num;idx++)
		{
			handle_t handle = ep_evts[idx].data.fd;
			assert(handlers_map->find(handle) != handlers_map->end());
			if(ep_evts[idx].events & EPOLLIN)
				(*handlers_map)[handle]->handle_read();
			if(ep_evts[idx].events & EPOLLOUT)
				(*handlers_map)[handle]->handle_write();
		}
	}
	return num;

}

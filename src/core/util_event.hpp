#pragma once

#include "error.hpp"
#include "system_compiler.hpp"
#include "util_function.hpp"

namespace cl3
{
	namespace util
	{
		namespace event
		{
			template<typename... Args>
			class CL3PUBT TEvent
			{
				public:
					class CL3PUBT TListener
					{
						friend class TEvent;
						protected:
							TEvent* event;
							TListener* previous;
							TListener* next;

						public:
							function::TFunction<void, Args...> callback;

							CL3PUBF CLASS TListener(const TListener&) = delete;

							CL3PUBF CLASS TListener(TListener&& other) : event(other.event), previous(other.previous), next(other.next), callback(other.callback)
							{
								other.event = NULL;
								other.previous = NULL;
								other.next = NULL;

								if(this->previous)
									this->previous->next = this;
								else
									this->event->first = this;

								if(this->next)
									this->next->previous = this;
								else
									this->event->last = this;
							}

							CL3PUBF CLASS TListener(TEvent* event, function::TFunction<void, Args...> callback) : event(event), previous(event->last), next(NULL), callback(callback)
							{
								this->event->last = this;

								if(this->event->first == NULL)
									this->event->first = this;

								if(this->previous)
									this->previous->next = this;
							}

							CL3PUBF CLASS ~TListener()
							{
								if(this->event)
								{
									if(this->previous)
										this->previous->next = this->next;
									else
										this->event->first = this->next;

									if(this->next)
										this->next->previous = this->previous;
									else
										this->event->last = this->previous;
								}
							}
					};

				protected:
					TListener* first;
					TListener* last;

				public:
					CL3PUBF CLASS TEvent() : first(NULL), last(NULL) {}

					CL3PUBF void Raise(Args... args)
					{
						for(TListener* p = this->first; p != NULL; p = p->next)
							p->callback(args...);
					}

					CL3PUBF TListener Register(function::TFunction<void, Args...> callback)
					{
						return TListener(this, callback);
					}
			};
		}
	}
}

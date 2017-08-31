FROM alpine:3.6
RUN apk update && apk upgrade && apk add bash
ADD ./infrabox /cl3/infrabox
CMD /cl3/infrabox/@/generator.sh /cl3

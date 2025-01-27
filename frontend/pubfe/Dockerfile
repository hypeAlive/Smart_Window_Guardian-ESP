FROM node:alpine AS build

WORKDIR /app
COPY package.json /app
RUN npm i

COPY src/ /app/src/
COPY *.json /app
COPY index.html /app
COPY env.d.ts /app
COPY vite.config.ts /app
COPY public/ /app/public/

RUN npm run build

FROM nginx:alpine

COPY --from=build /app/dist /usr/share/nginx/html

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]
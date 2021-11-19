#!/usr/bin/env python

import gitlab
import time, timeit
import sys
import os

from datetime import timedelta

ci_job_token=os.environ['CI_JOB_TOKEN']

gl = gitlab.Gitlab("https://gitlab.com/", job_token=ci_job_token)
#gl = gitlab.Gitlab("https://gitlab.com/", private_token="m2QBuzJHbb5thdr5xvdc")

container_project = os.environ['CONTAINER_PROJECT']
container_project_branch = os.environ['CONTAINER_PROJECT_BRANCH']

print("\nWorking with container project {}".format(container_project))
print("\nWorking with container project branch {}".format(container_project_branch))

project = gl.projects.get(container_project, lazy=True)
#trigger_pipeline = project.pipelines.create({'ref': container_project_branch})

# Set default
status = "pending"
start_time = timeit.default_timer()

#def get_or_create_trigger(project):
#    trigger_decription = 'rebuild-container'
#    for t in project.triggers.list():
#        if t.description == trigger_decription:
#            return t
#    return project.triggers.create({'description': trigger_decription})

#trigger = get_or_create_trigger(project)
#pipeline = project.trigger_pipeline(container_project_branch, trigger.token)
trigger_pipeline = project.trigger_pipeline(container_project_branch, ci_job_token)

#while pipeline.finished_at is None:
#    pipeline.refresh()
#    time.sleep(1)


while (status == "running" or status == "pending"):
    pipeline = project.pipelines.get(trigger_pipeline.id)

    status = pipeline.status

    elapsed_time = timeit.default_timer() - start_time
    formated_time = str(timedelta(seconds=elapsed_time))
    sys.stderr.write("Still running pipeline... ({})\n".format(formated_time))

    if status == "success":
        sys.stderr.write("\nPipeline success\n")
        break
    elif status == "failed":
        raise Exception
    elif status == "canceled":
        raise Exception

    time.sleep(10)

